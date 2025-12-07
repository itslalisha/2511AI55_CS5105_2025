import os
import logging
from fpdf import FPDF
import pandas as pd

# --- Logger Setup ---
# Logs errors to 'system_errors.log'
logging.basicConfig(
    filename='system_errors.log',
    level=logging.ERROR,
    format='%(asctime)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

class AttendancePDF(FPDF):
    def header(self):
        self.set_font('Arial', 'B', 16)
        self.cell(0, 10, 'IITP Attendance System', 0, 1, 'C')
        self.ln(5)

    def footer(self):
        self.set_y(-15)
        self.set_font('Arial', 'I', 8)
        self.cell(0, 10, f'Page {self.page_no()}', 0, 0, 'C')

def generate_pdf(group_data, date_obj, session, room, subject_code, photos_dir, output_folder):
    """
    Generates PDF with format: 2025_10_31_Morning_R104_CE1101.pdf
    """
    try:
        # 1. Format Date for Filename (YYYY_MM_DD)
        # Handles both datetime objects and strings
        if isinstance(date_obj, str):
            try:
                # Try parsing if it's a string
                dt = pd.to_datetime(date_obj)
                date_str = dt.strftime('%Y_%m_%d')
                display_date = dt.strftime('%d-%m-%Y')
            except:
                # Fallback if parsing fails
                date_str = date_obj.replace('-', '_').replace('/', '_')
                display_date = date_obj
        else:
            date_str = date_obj.strftime('%Y_%m_%d')
            display_date = date_obj.strftime('%d-%m-%Y')

        # 2. Construct Filename
        filename = f"{date_str}_{session}_{room}_{subject_code}.pdf"
        output_path = os.path.join(output_folder, filename)

        pdf = AttendancePDF()
        pdf.add_page()
        
        # 3. Header Info Block
        pdf.set_font('Arial', '', 12)
        student_count = len(group_data)
        
        # Matching the style of your uploaded sample PDF
        info_text = (f"Date: {display_date} | Shift: {session} | Room No: {room} | "
                     f"Student count: {student_count}\nSubject: {subject_code}")
        pdf.multi_cell(0, 8, info_text, border=1, align='L')
        pdf.ln(5)

        # 4. Student Grid
        col_width = 45
        row_height = 40
        x_start = 10
        y_start = pdf.get_y()
        x, y = x_start, y_start
        
        for index, row in group_data.iterrows():
            roll = str(row['Roll'])
            name = str(row['Name'])
            
            # Check for page break
            if y + row_height > 270:
                pdf.add_page()
                y = 20
                x = x_start

            # --- Image Handling ---
            photo_path = os.path.join(photos_dir, f"{roll}.jpg")
            
            pdf.rect(x, y, col_width, row_height) # Cell Border
            
            if os.path.exists(photo_path):
                try:
                    pdf.image(photo_path, x=x+5, y=y+2, w=35, h=25) 
                except Exception as img_err:
                    # Log specific image error with DATE context
                    logging.error(f"Date: {display_date} | Room: {room} | Roll: {roll} - Corrupt Image. Error: {img_err}")
                    pdf.set_xy(x, y+10)
                    pdf.set_font('Arial', 'I', 8)
                    pdf.cell(col_width, 5, "Image Error", 0, 0, 'C')
            else:
                # Placeholder for missing image
                pdf.set_xy(x, y+10)
                pdf.set_font('Arial', 'I', 8)
                pdf.cell(col_width, 5, "No Image", 0, 0, 'C')
                # Optional: Log missing image
                # logging.warning(f"Date: {display_date} | Room: {room} | Roll: {roll} - Image Missing")

            # --- Text Details ---
            pdf.set_xy(x, y+28)
            pdf.set_font('Arial', 'B', 8)
            pdf.cell(col_width, 4, name[:20], 0, 2, 'C') # Truncate long names
            
            pdf.set_font('Arial', '', 8)
            pdf.cell(col_width, 4, f"{roll}", 0, 2, 'C')
            pdf.cell(col_width, 4, "Sign: _______", 0, 0, 'C')

            # Move to next cell
            x += col_width
            if x > 180: # End of row
                x = x_start
                y += row_height + 5

        pdf.output(output_path)
        return filename

    except Exception as e:
        # Log critical generation errors with DATE context
        logging.error(f"FAILED TO GENERATE | Date: {date_str} | Session: {session} | Room: {room} | Subject: {subject_code} | Error: {e}")
        raise e