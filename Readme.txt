# 📄 Steps to Execute Student Grouping Streamlit App

## 1. Install Python
- Make sure **Python 3.8+** is installed on your system.
- Verify by running:
  python --version
  or  
  python3 --version

## 2. Create a Project Folder
- Create a new folder for your project, for example:
  How_to_Execute/

- Place your Python file (example: tut01.py) inside this folder.

## 3. Use any code editor


## 4. Install Required Libraries
Run the following command inside your project folder:
pip install streamlit pandas openpyxl

## 5.Ready Input file to giving input


## 6. Run the App
Run the following command in your terminal:
streamlit run tut01.py

## 7. Open in Browser
- Streamlit will open automatically in your browser at:
  http://localhost:8501
- If not, copy-paste the URL shown in terminal.

## 8. Use the App
1. Upload your CSV/Excel file in the app.
2. View summary of students and branches.
3. Choose the number of groups.
4. Click the buttons to:
   - Save branch files
   - Create branch-wise groups
   - Create uniform groups

## 9. Output Files
After execution, files will be saved in folders inside your project directory:

student_grouping_project/
├── branch_files/          # CSV files for each branch
├── branchwise_groups/     # Groups with round-robin distribution
│   ├── G1.csv
│   ├── G2.csv
│   └── stats_branchwise.csv
└── uniform_groups/        # Balanced groups
    ├── G1.csv
    ├── G2.csv
    └── stats_uniform.csv

✅ Done! You have successfully executed the Student Grouping App.
