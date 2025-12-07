#!/usr/bin/env python3
"""
Exam seating project - CLI version

Usage (Windows PowerShell / CMD):
    python exam_seating_project.py ^
        --input input_data_tt.xlsx ^
        --buffer 5 ^
        --density sparse ^
        --output-root output

What this generates:

output/
    op_overall_seating_arrangement.xlsx
    op_seats_left.xlsx
    01_05_2016/
        Morning/
            01_05_2016_CS249_LT103_morning.xlsx
            ...
        Evening/
            ...

Excel formats:

op_overall_seating_arrangement.xlsx
    Date | Day | course_code | Room | Allocated_students_count | Roll_list (semicolon separated)

op_seats_left.xlsx
    Room No. | Exam Capacity | Block | Alloted | Vacant (B-C)

Per-room sheet (e.g. 22_02_2025_CS2202_LT103_morning.xlsx)
    Row 1: "Course: CS2202 | Room: LT103 | Date: 22-02-2025 | Session: Morning"
    Row 2: headers: Roll | Student Name | Signature
    Next rows: one student per row
"""

import argparse
import logging
import os
import sys
from collections import defaultdict
from typing import Any, Dict, List, Set, Tuple

import pandas as pd


# ---------------------- Logging ---------------------- #

def setup_logging() -> logging.Logger:
    logger = logging.getLogger("exam_seating")
    logger.setLevel(logging.DEBUG)
    logger.handlers.clear()

    # execution log
    exec_handler = logging.FileHandler("execution.log", mode="w", encoding="utf-8")
    exec_handler.setLevel(logging.INFO)
    exec_formatter = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")
    exec_handler.setFormatter(exec_formatter)
    logger.addHandler(exec_handler)

    # errors
    err_handler = logging.FileHandler("errors.txt", mode="w", encoding="utf-8")
    err_handler.setLevel(logging.ERROR)
    err_handler.setFormatter(exec_formatter)
    logger.addHandler(err_handler)

    # console (info)
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(logging.INFO)
    console_handler.setFormatter(exec_formatter)
    logger.addHandler(console_handler)

    return logger


# ---------------------- Helpers ---------------------- #

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


def build_rooms(df_rooms: pd.DataFrame, buffer: int, density: str,
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
        except Exception:
            continue

        base_cap = max(0, exam_cap - buffer)
        if density == "dense":
            eff = base_cap
        else:  # sparse
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


def check_clashes(date_str: str,
                  slot_name: str,
                  course_codes: List[str],
                  global_course_rolls: Dict[str, Set[str]],
                  logger: logging.Logger) -> None:
    n = len(course_codes)
    logger.info("Checking clashes for %s %s", date_str, slot_name)
    for i in range(n):
        c1 = course_codes[i]
        r1 = global_course_rolls.get(c1, set())
        for j in range(i + 1, n):
            c2 = course_codes[j]
            r2 = global_course_rolls.get(c2, set())
            common = r1.intersection(r2)
            if common:
                msg = (f"CLASH on {date_str} {slot_name}: {c1} vs {c2}, "
                       f"roll(s): {', '.join(sorted(common))}")
                print(msg)
                logger.error(msg)


def check_total_capacity(rooms: List[Dict[str, Any]],
                         slot_courses: List[str],
                         global_course_rolls: Dict[str, Set[str]],
                         logger: logging.Logger) -> bool:
    total_cap = sum(r["effective_capacity"] for r in rooms)
    total_students = sum(len(global_course_rolls.get(c, set()))
                         for c in slot_courses)

    logger.info("Total students in slot = %d, total capacity = %d",
                total_students, total_cap)

    if total_students > total_cap:
        msg = (f"Cannot allocate due to excess students "
               f"(students = {total_students}, capacity = {total_cap})")
        print(msg)
        logger.error(msg)
        return False
    return True


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
        overall_rows  : list[dict] for op_overall_seating_arrangement (for this slot)
        room_usage    : dict[room_no] -> used_count (for this slot)
        assignments   : dict[(room_no, course)] -> list[(roll, name)]
    """
    logger.info("Allocating slot: %s %s (%s), courses: %s",
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

        # Try to fit in a single block
        for block, block_rooms in rooms_by_block.items():
            free_rooms = [r for r in block_rooms if not r["used"]]
            if not free_rooms:
                continue
            free_rooms.sort(key=lambda r: r["effective_capacity"], reverse=True)

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
                print(msg)
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
            print(msg)
            logger.error(msg)

    # Build overall_rows & room_usage
    overall_rows: List[Dict[str, Any]] = []
    room_usage: Dict[str, int] = {}

    for (room_no, course), stu_list in assignments.items():
        rolls = [s[0] for s in stu_list]
        cnt = len(rolls)
        rinfo = room_by_id[room_no]

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


def write_room_excels(output_root: str,
                      date_folder: str,
                      date_disp: str,
                      slot_name: str,
                      assignments: Dict[Tuple[str, str], List[Tuple[str, str]]]):
    """
    For each (room,course) create a file:
        <date>_<course>_<room>_<session>.xlsx
    under: output_root/date_folder/slot_name/
    """
    import openpyxl  # needed for header row formatting

    slot_folder = os.path.join(output_root, date_folder, slot_name)
    os.makedirs(slot_folder, exist_ok=True)

    for (room_no, course), stu_list in assignments.items():
        stu_list_sorted = sorted(stu_list, key=lambda x: x[0])  # by roll

        df = pd.DataFrame(
            [{"Roll": r, "Student Name": n, "Signature": ""} for r, n in stu_list_sorted]
        )

        fname = f"{date_folder}_{course}_{room_no}_{slot_name.lower()}.xlsx"
        fpath = os.path.join(slot_folder, fname)

        # Write DataFrame starting from row 2, then add header in row 1
        with pd.ExcelWriter(fpath, engine="openpyxl") as writer:
            df.to_excel(writer, index=False, startrow=1)
            ws = writer.sheets["Sheet1"]
            header_text = (f"Course: {course} | Room: {room_no} | "
                           f"Date: {date_disp} | Session: {slot_name}")
            ws["A1"] = header_text
            # merge A1:C1
            ws.merge_cells(start_row=1, start_column=1, end_row=1, end_column=3)


# ---------------------- Main ---------------------- #

def main():
    logger = setup_logging()
    logger.info("Starting exam seating project")

    parser = argparse.ArgumentParser(description="Optimal seating arrangement generator")
    parser.add_argument("--input", "-i", default="input_data.xlsx",
                        help="Input Excel file")
    parser.add_argument("--buffer", "-b", type=int, default=0,
                        help="Buffer seats per room")
    parser.add_argument("--density", "-d", choices=["Sparse", "Dense"],
                        default="Dense",
                        help="sparse = 50%% of (capacity-buffer), dense = full")
    parser.add_argument("--output-root", "-o", default="output",
                        help="Root output folder")

    args = parser.parse_args()

    try:
        if not os.path.exists(args.input):
            msg = f"Input file '{args.input}' not found."
            print(msg)
            logger.error(msg)
            sys.exit(1)

        xls = pd.ExcelFile(args.input)
        required = [
            "in_timetable",
            "in_course_roll_mapping",
            "in_roll_name_mapping",
            "in_room_capacity",
        ]
        for s in required:
            if s not in xls.sheet_names:
                msg = f"Missing sheet '{s}' in input Excel."
                print(msg)
                logger.error(msg)
                sys.exit(1)

        df_tt = xls.parse("in_timetable")
        df_course_roll = xls.parse("in_course_roll_mapping")
        df_roll_name = xls.parse("in_roll_name_mapping")
        df_rooms = xls.parse("in_room_capacity")

        name_map = build_name_map(df_roll_name, logger)
        global_course_rolls = build_course_rolls(df_course_roll, logger)

        # global outputs
        global_overall_rows: List[Dict[str, Any]] = []
        # room_no -> {exam_capacity, block, used_max}
        global_room_usage: Dict[str, Dict[str, Any]] = {}

        for _, row in df_tt.iterrows():
            date_val = row["Date"]
            if pd.isna(date_val):
                continue

            if hasattr(date_val, "strftime"):
                date_folder = date_val.strftime("%d_%m_%Y")
                date_disp = date_val.strftime("%d-%m-%Y")
            else:
                # treat as string already dd/mm/yyyy
                tmp = normalize_str(date_val)
                date_disp = tmp
                date_folder = tmp.replace("-", "_").replace("/", "_")

            day_str = normalize_str(row["Day"])

            for slot_name in ["Morning", "Evening"]:
                slot_raw = row.get(slot_name, "")
                slot_str = normalize_str(slot_raw)
                if not slot_str or slot_str.upper() == "NO EXAM":
                    logger.info("No exam on %s %s", date_disp, slot_name)
                    continue

                slot_courses = [c.strip() for c in slot_str.split(";")
                                if c and c.strip()]

                logger.info("Date %s, %s: courses=%s",
                            date_disp, slot_name, ", ".join(slot_courses))

                check_clashes(date_disp, slot_name,
                              slot_courses, global_course_rolls, logger)

                slot_rooms = build_rooms(df_rooms, args.buffer,
                                         args.density, logger)

                if not check_total_capacity(slot_rooms, slot_courses,
                                            global_course_rolls, logger):
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

                # append to global overall
                global_overall_rows.extend(overall_rows)

                # update global room usage (for seats_left)
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

                # write per-room Excel files
                write_room_excels(args.output_root,
                                  date_folder,
                                  date_disp,
                                  slot_name,
                                  assignments)

        # ---- write root-level Excel files ---- #
        os.makedirs(args.output_root, exist_ok=True)

        # overall_seating_arrangement.xlsx
        df_overall = pd.DataFrame(global_overall_rows)
        overall_path = os.path.join(args.output_root,
                                    "overall_seating_arrangement.xlsx")
        with pd.ExcelWriter(overall_path, engine="openpyxl") as writer:
            df_overall.to_excel(writer, index=False)

        # seats_left.xlsx
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
        # sort by block then room
        def sort_key(r):
            rn = r["Room No."]
            try:
                return r["Block"], int(rn)
            except Exception:
                return r["Block"], rn

        df_seats = df_seats.sort_values(
            by=["Block", "Room No."],
            key=lambda col: col.map(lambda x: int(x) if str(x).isdigit() else x),
        )

        seats_path = os.path.join(args.output_root, "seats_left.xlsx")
        with pd.ExcelWriter(seats_path, engine="openpyxl") as writer:
            df_seats.to_excel(writer, index=False)

        logger.info("Completed successfully.")

    except Exception:
        logger.exception("Unexpected error occurred.")
        print("Unexpected error occurred. Check errors.txt for details.")
        sys.exit(1)


if __name__ == "__main__":
    main()
