# BTP/MTP Faculty Allocator

This project provides a Streamlit web application to allocate students to faculties based on CGPA and preferences, using a cyclic "mod n" allocation logic.

## Project Structure 
. ├── app.py # The main Streamlit application ├── Dockerfile # Docker image definition ├── docker-compose.yml # Docker compose setup ├── requirements.txt # Python dependencies ├── app.log # Log file (will be created on run) └── README.md # This file


## Input File Format

The application expects a CSV file with the following column structure:

-   Any columns (e.g., `Roll No`, `Name`)
-   A `CGPA` column (must be named exactly this)
-   One or more preference columns immediately following `CGPA` (e.g., `Pref 1`, `Pref 2`, `Pref 3`)

**Example `input.csv`:**

| Roll No | Name | CGPA | Pref 1 | Pref 2 | Pref 3 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| 101 | Alice | 9.5 | FacA | FacB | FacC |
| 102 | Bob | 9.2 | FacA | FacC | FacB |
| 103 | Charlie | 9.8 | FacB | FacA | FacC |
| 104 | David | 8.5 | FacC | FacB | FacA |
| 105 | Eve | 9.2 | FacA | FacB | FacC |
| 106 | Frank | 8.8 | FacB | FacC | FacA |

---

## How to Run (Fulfills Requirement #3)

You can run this application in two ways:

### Method A: Running with Docker (Recommended)

This is the easiest way to run the application with all dependencies managed.

1.  **Build and run the container:**
    Open a terminal in the project directory and run:
    ```sh
    docker-compose up --build
    ```

2.  **Access the application:**
    Open your web browser and go to: `http://localhost:8501`

3.  **To stop the application:**
    Press `Ctrl+C` in your terminal.

### Method B: Running without Docker (Locally)

This method requires you to have Python and `pip` installed on your system.

1.  **Create a virtual environment (Recommended):**
    ```sh
    python -m venv venv
    source venv/bin/activate  # On Windows: venv\Scripts\activate
    ```

2.  **Install the required libraries:**
    ```sh
    pip install -r requirements.txt
    ```

3.  **Run the Streamlit app:**
    ```sh
    streamlit run app.py
    ```

4.  **Access the application:**
    Streamlit will automatically open the application in your web browser (usually at `htt