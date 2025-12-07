import streamlit as st
import pandas as pd
import logging
import io
import zipfile
import os
from collections import defaultdict
from typing import Any, Dict, List, Set, Tuple

# PDF generation
from reportlab.lib.pagesizes import A4, landscape
from reportlab.lib.units import mm
from reportlab.pdfgen import canvas


# ---------------------- Logging ---------------------- #

def setup_logging():
    """
    Creates a logger that:
    - writes ERRORs to errors.txt (as required)
    - writes INFO+ to an in-memory buffer we show in the UI
    """
    logger = logging.getLogger("exam_seating_streamlit")
    logger.setLevel(logging.DEBUG)
    logger.handlers.clear()

    fmt = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")

    # errors to file
    err_handler = logging.FileHandler("errors.txt", mode="w", encoding="utf-8")
    err_handler.setLevel(logging.ERROR)
    err_handler.setFormatter(fmt)
    logger.addHandler(err_handler)

    # info+ to buffer for UI
    log_buffer = io.StringIO()
    buf_handler = logging.StreamHandler(log_buffer)
    buf_handler.setLevel(logging.INFO)
    buf_handler.setFormatter(fmt)
    logger.addHandler(buf_handler)

    return logger, log_buffer


# ---------------------- Core helpers ---------------------- #

def normalize_str(x: Any) -> str:
    if pd.isna(x):
        return ""
    return str(x).strip()


def build_name_map(df_roll_name: pd.DataFrame, logger: logging.Logger) -> Dict[str, str]:
    df = df_roll_name.copy()
    df["Roll"] = df["Roll"].apply(normalize_str)
    df["Name"] = df["Name"].apply(normalize_str)

    name_map: Dict[str, str] = {}
    for _, row in df.iterrows():
        roll = row["Roll"]
        name = row["Name"] or "Unknown Name"
        if roll:
            name_map[roll] = name
    logger.info("Built roll-name mapping for %d students", len(name_map))
    return name_map


def build_course_rolls(df_course_roll: pd.DataFrame,
                       logger: logging.Logger) -> Dict[str, Set[str]]:
    df = df_course_roll.copy()
    df["rollno"] = df["rollno"].apply(normalize_str)
    df["course_code"] = df["course_code"].apply(normalize_str)

    course_to_rolls: Dict[str, Set[str]] = defaultdict(set)
    for _, row in df.iterrows():
        roll = row["rollno"]
        course = row["course_code"]
        if course and roll:
            course_to_rolls[course].add(roll)
    logger.info("Built course-roll mapping for %d courses", len(course_to_rolls))
    return course_to_rolls


def build_rooms(df_rooms: pd.DataFrame, buffer_val: int, density: str,
                logger: logging.Logger) -> List[Dict[str, Any]]:
    rooms: List[Dict[str, Any]] = []
    df = df_rooms.copy()
    df["Room No."] = df["Room No."].apply(normalize_str)
    df["Block"] = df["Block"].apply(normalize_str)

    for _, row in df.iterrows():
        room_no = row["Room No."]
        if not room_no:
            continue
        try:
            exam_cap = int(row["Exam Capacity"])
        except Exception as e:
            logger.error("Invalid Exam Capacity for room %s: %s", room_no, e)
            continue

        base_cap = max(0, exam_cap - buffer_val)
        if density == "dense":
            eff = base_cap
        else:   # sparse
            eff = base_cap // 2

        if eff <= 0:
            continue

        rooms.append({
            "room_no": room_no,
            "block": row["Block"],
            "exam_capacity": exam_cap,
            "effective_capacity": eff,
            "used": False,
        })

    def room_key(r: Dict[str, Any]) -> Tuple[str, int]:
        try:
            return r["block"], int(r["room_no"])
        except Exception:
            return r["block"], 0

    rooms.sort(key=room_key)
    logger.info("Prepared %d rooms after buffer/density", len(rooms))
    return rooms


def check_clashes(date_disp: str,
                  slot_name: str,
                  course_codes: List[str],
                  global_course_rolls: Dict[str, Set[str]],
                  logger: logging.Logger) -> List[str]:
    logger.info("Checking clashes for %s %s", date_disp, slot_name)
    clashes = []
    n = len(course_codes)
    for i in range(n):
        c1 = course_codes[i]
        r1 = global_course_rolls.get(c1, set())
        for j in range(i + 1, n):
            c2 = course_codes[j]
            r2 = global_course_rolls.get(c2, set())
            common = r1.intersection(r2)
            if common:
                msg = (f"CLASH on {date_disp} {slot_name}: {c1} vs {c2}, "
                       f"roll(s): {', '.join(sorted(common))}")
                logger.error(msg)
                clashes.append(msg)
    return clashes


def check_total_capacity(rooms: List[Dict[str, Any]],
                         slot_courses: List[str],
                         global_course_rolls: Dict[str, Set[str]],
                         logger: logging.Logger) -> Tuple[bool, str]:
    total_cap = sum(r["effective_capacity"] for r in rooms)
    total_students = sum(len(global_course_rolls.get(c, set()))
                         for c in slot_courses)

    logger.info("Total students in slot = %d, total capacity = %d",
                total_students, total_cap)

    if total_students > total_cap:
        msg = (f"Cannot allocate due to excess students "
               f"(students = {total_students}, capacity = {total_cap})")
        logger.error(msg)
        return False, msg
    return True, ""


def allocate_slot(date_disp: str,
                  day_str: str,
                  slot_name: str,
                  slot_courses: List[str],
                  rooms: List[Dict[str, Any]],
                  global_course_rolls: Dict[str, Set[str]],
                  name_map: Dict[str, str],
                  logger: logging.Logger):
    """
    Returns:
        overall_rows  : list[dict] for op_overall_seating_arrangement (this slot)
        room_usage    : dict[room_no] -> used_count
        assignments   : dict[(room_no, course)] -> list[(roll, name)]
    """
    logger.info("Allocating slot %s %s (%s), courses: %s",
                date_disp, day_str, slot_name, ";".join(slot_courses))

    rooms_by_block: Dict[str, List[Dict[str, Any]]] = defaultdict(list)
    for r in rooms:
        rooms_by_block[r["block"]].append(r)
    room_by_id = {r["room_no"]: r for r in rooms}

    course_sizes = [(c, len(global_course_rolls.get(c, set())))
                    for c in slot_courses]
    course_sizes.sort(key=lambda x: x[1], reverse=True)
    logger.info("Course sizes: %s", course_sizes)

    assignments: Dict[Tuple[str, str], List[Tuple[str, str]]] = defaultdict(list)

    for course, size in course_sizes:
        if size == 0:
            logger.info("Course %s has 0 students; skipping.", course)
            continue

        rolls_sorted = sorted(global_course_rolls.get(course, set()))
        logger.info("Allocating course %s (%d students)", course, size)

        best_block = None
        best_rooms: List[Dict[str, Any]] = []

        # try to fit in a single block
        for block, block_rooms in rooms_by_block.items():
            free_rooms = [r for r in block_rooms if not r["used"]]
            if not free_rooms:
                continue
            free_rooms.sort(key=lambda r: r["effective_capacity"],
                            reverse=True)

            cum_cap = 0
            chosen = []
            for r in free_rooms:
                chosen.append(r)
                cum_cap += r["effective_capacity"]
                if cum_cap >= size:
                    break

            if cum_cap >= size:
                if best_block is None or len(chosen) < len(best_rooms):
                    best_block = block
                    best_rooms = chosen

        if best_block is not None:
            selected_rooms = best_rooms
            logger.info("Course %s placed in block %s rooms %s",
                        course, best_block,
                        [r["room_no"] for r in selected_rooms])
        else:
            # spread across blocks
            free_rooms_all = [r for r in rooms if not r["used"]]
            free_rooms_all.sort(key=lambda r: r["effective_capacity"],
                                reverse=True)
            cum_cap = 0
            selected_rooms = []
            for r in free_rooms_all:
                selected_rooms.append(r)
                cum_cap += r["effective_capacity"]
                if cum_cap >= size:
                    break
            if cum_cap < size:
                msg = (f"Cannot allocate due to excess students for course "
                       f"{course} on {date_disp} {slot_name}")
                logger.error(msg)
                continue

        selected_rooms.sort(key=lambda r: int(r["room_no"])
                            if r["room_no"].isdigit() else r["room_no"])

        idx = 0
        for r in selected_rooms:
            cap = r["effective_capacity"]
            while cap > 0 and idx < len(rolls_sorted):
                roll = rolls_sorted[idx]
                idx += 1
                cap -= 1
                r["used"] = True

                name = name_map.get(roll, "Unknown Name")
                assignments[(r["room_no"], course)].append((roll, name))

            if idx >= len(rolls_sorted):
                break

        if idx != len(rolls_sorted):
            msg = (f"Internal allocation mismatch for {course} on "
                   f"{date_disp} {slot_name}: "
                   f"assigned {idx}/{len(rolls_sorted)} students.")
            logger.error(msg)

    overall_rows: List[Dict[str, Any]] = []
    room_usage: Dict[str, int] = {}

    for (room_no, course), stu_list in assignments.items():
        rolls = [s[0] for s in stu_list]
        cnt = len(rolls)
        overall_rows.append({
            "Date": date_disp,
            "Day": day_str,
            "course_code": course,
            "Room": room_no,
            "Allocated_students_count": cnt,
            "Roll_list (semicolon separated)": ";".join(rolls),
        })
        room_usage[room_no] = room_usage.get(room_no, 0) + cnt

    return overall_rows, room_usage, assignments


# ---------------------- Excel & PDF helpers ---------------------- #

def room_excel_bytes(date_folder: str,
                     date_disp: str,
                     slot_name: str,
                     room_no: str,
                     course: str,
                     stu_list: List[Tuple[str, str]]) -> Tuple[str, bytes]:
    """Create bytes for one room Excel file with header row like sample."""
    stu_list_sorted = sorted(stu_list, key=lambda x: x[0])
    df = pd.DataFrame(
        [{"Roll": r, "Student Name": n, "Signature": ""} for r, n in stu_list_sorted]
    )

    bio = io.BytesIO()
    with pd.ExcelWriter(bio, engine="openpyxl") as writer:
        df.to_excel(writer, index=False, startrow=1)
        ws = writer.sheets["Sheet1"]
        header_text = (f"Course: {course} | Room: {room_no} | "
                       f"Date: {date_disp} | Session: {slot_name}")
        ws["A1"] = header_text
        ws.merge_cells(start_row=1, start_column=1, end_row=1, end_column=3)

    bio.seek(0)
    fname = f"{date_folder}_{course}_{room_no}_{slot_name.lower()}.xlsx"
    inner_path = f"{date_folder}/{slot_name}/{fname}"
    return inner_path, bio.read()


def build_attendance_pdf_bytes(date_disp: str,
                               slot_name: str,
                               room_no: str,
                               course: str,
                               stu_list: List[Tuple[str, str]],
                               photos_root: str,
                               logger: logging.Logger) -> Tuple[str, bytes]:
    """
    Build an attendance PDF matching the IITP sample layout:
    - Outer border + title
    - Header row: Date / Shift / Room / Student count
    - Subject row
    - 3-column grid of students
    - Bottom table: Invigilator Name & Signature (Sl No / Name / Signature)
    """
    buf = io.BytesIO()
    c = canvas.Canvas(buf, pagesize=landscape(A4))
    width, height = landscape(A4)

    margin = 10 * mm
    left = margin
    right = width - margin
    bottom = margin
    top = height - margin

    # -------- Outer border -------- #
    c.setLineWidth(1.5)
    c.rect(left, bottom, right - left, top - bottom)

    # -------- Title -------- #
    c.setFont("Helvetica-Bold", 20)
    c.drawCentredString(width / 2, top - 8 * mm, "IITP Attendance System")

    # -------- Header: Date / Shift / Room / Count -------- #
    c.setLineWidth(1.0)
    header_top = top - 15 * mm
    header_bottom = header_top - 8 * mm

    c.setFont("Helvetica-Bold", 10)
    header_text_1 = (
        f"Date: {date_disp} | Shift: {slot_name} | "
        f"Room No: {room_no} | Student count: {len(stu_list)}"
    )
    c.drawString(left + 2 * mm, header_bottom + 2 * mm, header_text_1)
    c.line(left, header_bottom, right, header_bottom)

    # -------- Subject row -------- #
    subj_top = header_bottom
    subj_bottom = subj_top - 8 * mm
    c.setFont("Helvetica-Bold", 10)
    subj_text = f"Subject: {course} | Stud Present: | Stud Absent:"
    c.drawString(left + 2 * mm, subj_bottom + 2 * mm, subj_text)
    c.line(left, subj_bottom, right, subj_bottom)

    # -------- Grid area (3 columns) -------- #
    cols = 3
    rows_per_page = 4          # keep some space for bottom table
    col_w = (right - left) / cols

    grid_top = subj_bottom
    invigilator_block_height = 40 * mm
    grid_bottom = bottom + invigilator_block_height

    available_grid_height = grid_top - grid_bottom
    row_h = available_grid_height / rows_per_page

    c.setFont("Helvetica", 9)
    c.setLineWidth(0.8)

    def draw_student_cell(idx: int, roll: str, name: str):
        page_index = idx // (cols * rows_per_page)
        idx_in_page = idx % (cols * rows_per_page)
        row = idx_in_page // cols
        col = idx_in_page % cols

        # new page if needed
        if idx_in_page == 0 and page_index > 0:
            c.showPage()
            # redraw frame + header for new page
            c.setLineWidth(1.5)
            c.rect(left, bottom, right - left, top - bottom)
            c.setFont("Helvetica-Bold", 20)
            c.drawCentredString(width / 2, top - 8 * mm, "IITP Attendance System")

            c.setLineWidth(1.0)
            c.setFont("Helvetica-Bold", 10)
            c.drawString(left + 2 * mm, header_bottom + 2 * mm, header_text_1)
            c.line(left, header_bottom, right, header_bottom)

            c.drawString(left + 2 * mm, subj_bottom + 2 * mm, subj_text)
            c.line(left, subj_bottom, right, subj_bottom)

            c.setFont("Helvetica", 9)
            c.setLineWidth(0.8)

        cell_top = grid_top - row * row_h
        cell_bottom = cell_top - row_h
        cell_left = left + col * col_w
        cell_right = cell_left + col_w

        # cell border
        c.rect(cell_left, cell_bottom, col_w, row_h)

        # photo box
        photo_box_size = row_h - 8 * mm
        photo_left = cell_left + 2 * mm
        photo_bottom = cell_bottom + (row_h - photo_box_size) / 2

        text_left = photo_left + photo_box_size + 2 * mm
        c.line(text_left, cell_bottom, text_left, cell_top)

        photo_path = os.path.join(photos_root, f"{roll}.jpg")
        try:
            if os.path.exists(photo_path):
                c.drawImage(
                    photo_path,
                    photo_left,
                    photo_bottom,
                    width=photo_box_size,
                    height=photo_box_size,
                    preserveAspectRatio=True,
                    anchor='sw'
                )
            else:
                c.rect(photo_left, photo_bottom, photo_box_size, photo_box_size)
                c.setFont("Helvetica", 6)
                c.drawCentredString(
                    photo_left + photo_box_size / 2,
                    photo_bottom + photo_box_size / 2 + 3,
                    "No Image"
                )
                c.drawCentredString(
                    photo_left + photo_box_size / 2,
                    photo_bottom + photo_box_size / 2 - 4,
                    "Available"
                )
                c.setFont("Helvetica", 9)
        except Exception as e:
            logger.error("Error drawing photo for roll %s in room %s: %s",
                         roll, room_no, e)
            c.rect(photo_left, photo_bottom, photo_box_size, photo_box_size)

        # text
        text_x = text_left + 2 * mm
        text_y = cell_top - 6 * mm

        c.setFont("Helvetica-Bold", 9)
        c.drawString(text_x, text_y, name)
        c.setFont("Helvetica", 9)
        c.drawString(text_x, text_y - 5 * mm, f"Roll: {roll}")

        sign_y = text_y - 10 * mm
        c.drawString(text_x, sign_y, "Sign:")
        line_start = text_x + 18 * mm
        c.line(line_start, sign_y - 1, cell_right - 4 * mm, sign_y - 1)

    # draw all students
    for idx, (roll, name) in enumerate(stu_list):
        draw_student_cell(idx, roll, name)

    # -------- Invigilator table at bottom (this is the part you care about) -------- #
        # -------- Invigilator table at bottom (bigger rows) -------- #
    c.setFont("Helvetica-Bold", 10)

    # We compute table geometry first, then put the title just above it
    table_bottom = bottom + 5 * mm

    header_h = 7 * mm           # header row height
    row_h_table = 7 * mm        # EACH data row height (taller than before)
    data_rows = 6               # 6 invigilator rows

    total_h = header_h + data_rows * row_h_table
    table_top = table_bottom + total_h

    # Title centered above the table
    label_y = table_top + 3 * mm
    c.drawCentredString((left + right) / 2, label_y, "Invigilator Name & Signature")

    col_sl_w = 20 * mm
    col_sig_w = 40 * mm
    # name column width is implied
    x_sl = left + col_sl_w
    x_sig = right - col_sig_w

    # outer rectangle of table
    c.setLineWidth(1.0)
    c.rect(left, table_bottom, right - left, total_h)

    # vertical lines for columns
    c.line(x_sl, table_bottom, x_sl, table_top)
    c.line(x_sig, table_bottom, x_sig, table_top)

    # horizontal line separating header from rows
    header_y = table_top - header_h
    c.line(left, header_y, right, header_y)

    # horizontal lines for each row
    for r in range(data_rows):
        y = header_y - (r + 1) * row_h_table
        c.line(left, y, right, y)

    # header text: centered in header row cells
    c.setFont("Helvetica-Bold", 9)
    header_center_y = header_y + header_h / 2 - 3  # vertical centering tweak
    c.drawCentredString((left + x_sl) / 2, header_center_y, "Sl No.")
    c.drawCentredString((x_sl + x_sig) / 2, header_center_y, "Name")
    c.drawCentredString((x_sig + right) / 2, header_center_y, "Signature")

    # Sl No. values (1..6) nicely centered in their rows
    c.setFont("Helvetica", 9)
    for r in range(data_rows):
        row_center_y = header_y - r * row_h_table - row_h_table / 2 - 1
        c.drawCentredString((left + x_sl) / 2, row_center_y, str(r + 1))

    c.showPage()
    c.save()
    buf.seek(0)

    date_folder = date_folder_from_disp(date_disp)
    fname = f"{date_folder}_{course}_{room_no}_{slot_name.lower()}_attendance.pdf"
    inner_path = f"{date_folder}/{slot_name}/{fname}"
    return inner_path, buf.read()




def date_folder_from_disp(date_disp: str) -> str:
    """
    Convert 'dd-mm-yyyy' -> 'dd_mm_yyyy'
    """
    return date_disp.replace("-", "_").replace("/", "_")


def df_to_excel_bytes(df: pd.DataFrame) -> bytes:
    bio = io.BytesIO()
    with pd.ExcelWriter(bio, engine="openpyxl") as writer:
        df.to_excel(writer, index=False)
    bio.seek(0)
    return bio.read()


# ---------------------- Main allocation wrapper (Streamlit) ---------------------- #

def run_allocation_streamlit(file, buffer_val: int, density: str,
                             photos_root: str):
    logger, log_buf = setup_logging()

    try:
        xls = pd.ExcelFile(file)

        required = [
            "in_timetable",
            "in_course_roll_mapping",
            "in_roll_name_mapping",
            "in_room_capacity",
        ]
        for s in required:
            if s not in xls.sheet_names:
                msg = f"Missing sheet '{s}' in input Excel."
                logger.error(msg)
                return None, None, log_buf.getvalue(), msg

        df_tt = xls.parse("in_timetable")
        df_course_roll = xls.parse("in_course_roll_mapping")
        df_roll_name = xls.parse("in_roll_name_mapping")
        df_rooms = xls.parse("in_room_capacity")

        name_map = build_name_map(df_roll_name, logger)
        global_course_rolls = build_course_rolls(df_course_roll, logger)

        global_overall_rows: List[Dict[str, Any]] = []
        global_room_usage: Dict[str, Dict[str, Any]] = {}

        zip_buffer = io.BytesIO()
        zipf = zipfile.ZipFile(zip_buffer, "w", zipfile.ZIP_DEFLATED)

        # iterate over rows (days)
        for _, row in df_tt.iterrows():
            try:
                date_val = row["Date"]
                if pd.isna(date_val):
                    continue

                if hasattr(date_val, "strftime"):
                    date_folder = date_val.strftime("%d_%m_%Y")
                    date_disp = date_val.strftime("%d-%m-%Y")
                else:
                    tmp = normalize_str(date_val)
                    date_disp = tmp
                    date_folder = date_folder_from_disp(tmp)

                day_str = normalize_str(row["Day"])

                for slot_name in ["Morning", "Evening"]:
                    try:
                        slot_raw = row.get(slot_name, "")
                        slot_str = normalize_str(slot_raw)
                        if not slot_str or slot_str.upper() == "NO EXAM":
                            logger.info("No exam on %s %s", date_disp, slot_name)
                            continue

                        slot_courses = [c.strip() for c in slot_str.split(";")
                                        if c and c.strip()]
                        logger.info("Date %s, %s: courses=%s",
                                    date_disp, slot_name, ", ".join(slot_courses))

                        clashes = check_clashes(date_disp, slot_name,
                                                slot_courses, global_course_rolls, logger)
                        # clashes already logged; Streamlit will show logs

                        slot_rooms = build_rooms(df_rooms, buffer_val, density, logger)

                        ok, msg_cap = check_total_capacity(slot_rooms,
                                                           slot_courses,
                                                           global_course_rolls,
                                                           logger)
                        if not ok:
                            # skip this slot but continue other slots/days
                            continue

                        overall_rows, room_usage_slot, assignments = allocate_slot(
                            date_disp,
                            day_str,
                            slot_name,
                            slot_courses,
                            slot_rooms,
                            global_course_rolls,
                            name_map,
                            logger,
                        )

                        global_overall_rows.extend(overall_rows)

                        # room usage for op_seats_left
                        for r in slot_rooms:
                            room_no = r["room_no"]
                            exam_cap = r["exam_capacity"]
                            block = r["block"]
                            used_now = room_usage_slot.get(room_no, 0)

                            if room_no not in global_room_usage:
                                global_room_usage[room_no] = {
                                    "Exam Capacity": exam_cap,
                                    "Block": block,
                                    "used_max": used_now,
                                }
                            else:
                                global_room_usage[room_no]["used_max"] = max(
                                    global_room_usage[room_no]["used_max"],
                                    used_now,
                                )

                        # write room-wise Excel + PDF into ZIP
                        for (room_no, course), stu_list in assignments.items():
                            # Excel
                            try:
                                inner_path_xlsx, data_xlsx = room_excel_bytes(
                                    date_folder, date_disp, slot_name,
                                    room_no, course, stu_list
                                )
                                zipf.writestr(inner_path_xlsx, data_xlsx)
                            except Exception as e:
                                logger.error("Error creating Excel for %s-%s-%s: %s",
                                             date_disp, room_no, course, e)

                            # PDF
                            try:
                                inner_path_pdf, data_pdf = build_attendance_pdf_bytes(
                                    date_disp, slot_name, room_no, course,
                                    stu_list, photos_root, logger
                                )
                                zipf.writestr(inner_path_pdf, data_pdf)
                            except Exception as e:
                                logger.error("Error creating PDF for %s-%s-%s: %s",
                                             date_disp, room_no, course, e)

                    except Exception as e_slot:
                        # don't kill the app if one slot fails
                        logger.error("Error in date %s slot %s: %s",
                                     date_disp, slot_name, e_slot)

            except Exception as e_row:
                logger.error("Error processing timetable row: %s", e_row)

        # root-level overall & seats_left
        df_overall = pd.DataFrame(global_overall_rows)

        seats_rows = []
        for room_no, info in global_room_usage.items():
            exam_cap = info["Exam Capacity"]
            block = info["Block"]
            alloted = info["used_max"]
            vacant = exam_cap - alloted
            seats_rows.append({
                "Room No.": room_no,
                "Exam Capacity": exam_cap,
                "Block": block,
                "Alloted": alloted,
                "Vacant (B-C)": vacant,
            })
        df_seats = pd.DataFrame(seats_rows)

        if not df_overall.empty:
            zipf.writestr("op_overall_seating_arrangement.xlsx",
                          df_to_excel_bytes(df_overall))
        if not df_seats.empty:
            zipf.writestr("op_seats_left.xlsx",
                          df_to_excel_bytes(df_seats))

        zipf.close()
        zip_buffer.seek(0)

        return zip_buffer, (df_overall, df_seats), log_buf.getvalue(), ""

    except Exception as e:
        # final catch – log and return
        logger, lb = setup_logging()
        logger.exception("Unexpected error in Streamlit allocation.")
        return None, None, lb.getvalue(), str(e)


# ---------------------- Streamlit UI ---------------------- #

st.set_page_config(page_title="IITP Exam Seating Arrangement", layout="wide")

st.title("IITP Exam Seating Arrangement ")

st.markdown(
    """
Upload **input_data_tt.xlsx**, choose buffer & density, and the app will:

- Allocate rooms (largest course first, block-wise)
- Generate:
  - `op_overall_seating_arrangement.xlsx`
  - `op_seats_left.xlsx`
  - Per-room Excel sheets (Morning/Evening)
  - Per-room PDF **attendance sheets** using photos in `photos/ROLL.jpg`
- Package everything into **one ZIP**.
"""
)

uploaded = st.file_uploader("Upload Excel file (input_data_tt.xlsx)", type=["xlsx"])

col1, col2 = st.columns(2)
with col1:
    buffer_val = st.number_input("Buffer per classroom", min_value=0, max_value=100,
                                 value=5, step=1)
with col2:
    density = st.radio("Seat density mode",
                       options=["sparse", "dense"],
                       index=1,
                       help="sparse = 50% of (capacity - buffer), dense = full.")

photos_root = "photos"  # assume photos/ROLL.jpg exists and mounted in Docker

if st.button(" Generate Seating + PDFs"):
    if uploaded is None:
        st.error("Please upload the Excel file first.")
    else:
        if not os.path.isdir(photos_root):
            st.warning(f"Photos folder '{photos_root}' not found inside container. "
                       f"PDFs will still be created but will show 'No Image'.")
        st.info("Running allocation...")

        zip_buf, dfs, logs, err_msg = run_allocation_streamlit(
            uploaded, buffer_val, density, photos_root
        )

        st.subheader(" Execution Log")
        st.text_area("Logs", logs, height=250)

        if err_msg:
            st.error(f"Top-level error: {err_msg} (details in errors.txt)")

        if dfs is not None:
            df_overall, df_seats = dfs
            if df_overall is not None and not df_overall.empty:
                st.subheader(" Preview – overall_seating_arrangement")
                st.dataframe(df_overall.head(50))
            if df_seats is not None and not df_seats.empty:
                st.subheader(" Preview – seats_left")
                st.dataframe(df_seats.head(50))

        if zip_buf is not None:
            st.subheader(" Download ZIP (Excel + PDFs)")
            st.download_button(
                label="Download seating_outputs.zip",
                data=zip_buf.getvalue(),
                file_name="seating_outputs.zip",
                mime="application/zip",
            )

st.markdown("---")
st.caption("Errors are logged to errors.txt. Individual slot/room errors are caught so the app keeps running.")
