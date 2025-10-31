import streamlit as st
import pandas as pd
import logging
import io

# 4. Use logger library to record errors
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    filename='app.log'  # Logs to a file
)
logger = logging.getLogger(__name__)

def convert_df_to_csv(df):
    """Converts a DataFrame to a CSV string for download."""
    # 5. Use try catch whereever possible
    try:
        @st.cache_data
        def to_csv(df_data):
            # Use io.StringIO for in-memory text buffer
            output = io.StringIO()
            df_data.to_csv(output, index=False, encoding='utf-8')
            return output.getvalue()
        
        return to_csv(df)
    except Exception as e:
        logger.error(f"Error converting DataFrame to CSV: {e}")
        st.error(f"Error preparing CSV for download: {e}")
        return None

def generate_preference_stats(df, fac_cols, all_faculties):
    """
    Generates the faculty preference statistics (Output B).
    Input format: 'fac_cols' are faculty names, values are pref ranks.
    """
    # 5. Use try catch whereever possible
    try:
        logger.info("Generating faculty preference stats...")
        num_preferences = len(all_faculties)
        
        # Create column names like 'Count Pref 1', 'Count Pref 2', ...
        pref_count_cols = [f'Count Pref {i}' for i in range(1, num_preferences + 1)]
        
        # --- FIX is here ---
        # Convert the set to a list for the DataFrame index
        faculty_index = sorted(list(all_faculties))
        
        # Initialize DataFrame with 0s
        stats_df = pd.DataFrame(0, index=faculty_index, columns=pref_count_cols)

        # Iterate through each faculty column (e.g., 'ABM', 'AE')
        for fac_name in fac_cols:
            if fac_name in stats_df.index:
                # Get the counts of each preference rank (e.g., 5 students put '1', 3 students put '2')
                pref_ranks_counts = df[fac_name].value_counts()
                
                for pref_num, count in pref_ranks_counts.items():
                    # Ensure pref_num is an integer and within range
                    try:
                        pref_num_int = int(pref_num)
                        if 1 <= pref_num_int <= num_preferences:
                            col_name = f'Count Pref {pref_num_int}'
                            stats_df.loc[fac_name, col_name] = count
                    except ValueError:
                        logger.warning(f"Skipping non-numeric preference rank '{pref_num}' for faculty '{fac_name}'")
        
        # Add Total column
        stats_df['Total'] = stats_df.sum(axis=1)
        
        # Make Faculty a column
        stats_df = stats_df.reset_index().rename(columns={'index': 'Faculty'})
        
        logger.info("Successfully generated preference stats.")
        return stats_df

    except Exception as e:
        logger.error(f"Error in generate_preference_stats: {e}", exc_info=True)
        st.error(f"An error occurred while generating faculty stats: {e}")
        return pd.DataFrame()

def perform_allocation(df, fac_cols, all_faculties):
    """
    Performs the student allocation (Output A) based on:
    1. Sorted by CGPA (descending).
    2. "mod n" logic: In each "cycle", every faculty can be allocated at most one student.
       A cycle consists of checking preferences 1..n for all unallocated students.
    """
    # 5. Use try catch whereever possible
    try:
        logger.info("Starting student allocation...")
        
        # 2. Sort by CGPA at run time
        df_sorted = df.sort_values(by='CGPA', ascending=False)
        
        # --- Transform student data ---
        # Convert the wide preference format (cols=facs) to an ordered list of preferences
        students_data = []
        for _, row in df_sorted.iterrows():
            try:
                # Get the Series of preferences for this student (e.g., 'ABM': 14, 'AE': 11, ...)
                prefs_wide = row[fac_cols]
                
                # Sort by the values (the preference ranks 1, 2, 3...)
                # The index of the sorted Series is now the ordered list of faculty names
                prefs_sorted = prefs_wide.sort_values()
                
                # Create the ordered list of faculty names
                pref_list = list(prefs_sorted.index)
                
                students_data.append({
                    'Roll': row['Roll'],
                    'Name': row['Name'],
                    'Email': row['Email'],
                    'CGPA': row['CGPA'],
                    'preferences': pref_list # This list is ordered from Pref 1 to Pref N
                })
            except Exception as e:
                logger.error(f"Error processing student {row.get('Roll', 'UNKNOWN')}: {e}")
                
        # --- Run allocation logic ---
        student_allocated = {s['Roll']: False for s in students_data}
        final_allocations_list = []
        unallocated_count = len(students_data)
        num_preferences = len(all_faculties)
        
        cycle_num = 0
        while unallocated_count > 0:
            cycle_num += 1
            logger.info(f"Starting allocation cycle {cycle_num}. {unallocated_count} students remaining.")
            
            # 3A. "mod n" logic: Reset faculty availability for this cycle
            faculty_allocated_in_this_cycle = {fac: False for fac in all_faculties}
            students_allocated_this_cycle = 0
            
            # Iterate through preferences 1 to n
            for pref_index in range(num_preferences):
                # Iterate through students in CGPA-sorted order
                for student in students_data:
                    roll_no = student['Roll']
                    
                    # Skip if student is already allocated
                    if student_allocated[roll_no]:
                        continue
                    
                    # Get the student's N-th preference faculty
                    try:
                        faculty = student['preferences'][pref_index]
                    except IndexError:
                        continue # Student has fewer preferences than max

                    # Check if faculty is valid and NOT allocated in this cycle
                    if faculty in faculty_allocated_in_this_cycle and not faculty_allocated_in_this_cycle[faculty]:
                        # Allocate!
                        final_allocations_list.append({
                            'Roll': student['Roll'],
                            'Name': student['Name'],
                            'Email': student['Email'],
                            'CGPA': student['CGPA'],
                            'Allocated': faculty  # Match output file column name
                        })
                        
                        # Mark student as allocated
                        student_allocated[roll_no] = True
                        # Mark faculty as "used" for this cycle
                        faculty_allocated_in_this_cycle[faculty] = True
                        
                        unallocated_count -= 1
                        students_allocated_this_cycle += 1
            
            # Safety break: If no students were allocated in a full cycle
            if students_allocated_this_cycle == 0 and unallocated_count > 0:
                logger.warning(f"Could not allocate {unallocated_count} students. Ending allocation.")
                
                # Add unallocated students to the list
                for student in students_data:
                    if not student_allocated[student['Roll']]:
                         final_allocations_list.append({
                            'Roll': student['Roll'],
                            'Name': student['Name'],
                            'Email': student['Email'],
                            'CGPA': student['CGPA'],
                            'Allocated': 'UNALLOCATED'
                        })
                break # Exit while loop

        logger.info("Allocation complete.")
        # Re-sort by original CGPA order to be clean
        allocation_df = pd.DataFrame(final_allocations_list)
        return allocation_df
        
    except KeyError as e:
        logger.error(f"Missing expected column in allocation: {e}")
        st.error(f"Allocation failed. Missing required column: {e}. Ensure 'Roll', 'Name', 'Email', and 'CGPA' exist.")
        return pd.DataFrame()
    except Exception as e:
        logger.error(f"Error in perform_allocation: {e}", exc_info=True)
        st.error(f"An error occurred during allocation: {e}")
        return pd.DataFrame()

def main():
    st.set_page_config(layout="wide")
    st.title("BTP/MTP Faculty Allocator")

    # 1. Use Streamlit for uploading input file
    uploaded_file = st.file_uploader("Upload Student Preferences CSV (input_btp_mtp_allocation.csv)", type=["csv"])

    if uploaded_file is not None:
        try:
            df = pd.read_csv(uploaded_file)
            st.success("File uploaded successfully!")
            
            with st.expander("Preview Uploaded Data"):
                st.dataframe(df.head())
        
        except Exception as e:
            logger.error(f"Error reading uploaded CSV: {e}")
            st.error(f"Error reading file: {e}. Please ensure it's a valid CSV.")
            return

        # 5. Use try catch for main logic block
        try:
            # --- Column Identification ---
            # 1. Count the faculties dynamically post CGPA columns.
            
            if 'CGPA' not in df.columns:
                st.error("Input file MUST contain a 'CGPA' column.")
                logger.error("Input file missing 'CGPA' column.")
                return
            
            # Find all columns AFTER 'CGPA'
            all_cols = list(df.columns)
            cgpa_index = all_cols.index('CGPA')
            fac_cols = all_cols[cgpa_index + 1:] # These are the faculty names
            
            if not fac_cols:
                st.error("No faculty columns found after 'CGPA' column.")
                logger.error("No faculty columns found after 'CGPA'.")
                return
            
            all_faculties = set(fac_cols) # The set of all faculty names
            st.info(f"Identified {len(all_faculties)} Faculties: {', '.join(sorted(fac_cols))}")

            # --- Run Processes ---
            stats_df = generate_preference_stats(df, fac_cols, all_faculties)
            allocation_df = perform_allocation(df, fac_cols, all_faculties)

            # --- Display Outputs ---
            col1, col2 = st.columns(2)
            
            with col1:
                st.header("A) Student Allocation")
                st.dataframe(allocation_df)
                
                alloc_csv_data = convert_df_to_csv(allocation_df)
                if alloc_csv_data:
                    # 1. Use Streamlit for ... downloading output files
                    st.download_button(
                        label="Download Allocation CSV",
                        data=alloc_csv_data,
                        file_name="student_allocation.csv",
                        mime="text/csv",
                    )

            with col2:
                st.header("B) Faculty Preference Stats")
                st.dataframe(stats_df)
                
                stats_csv_data = convert_df_to_csv(stats_df)
                if stats_csv_data:
                    # 1. Use Streamlit for ... downloading output files
                    st.download_button(
                        label="Download Stats CSV",
                        data=stats_csv_data,
                        file_name="faculty_preference_stats.csv",
                        mime="text/csv",
                    )
        
        except Exception as e:
            logger.error(f"A critical error occurred in main processing: {e}", exc_info=True)
            st.error(f"A critical error occurred: {e}")

if __name__ == "__main__":
    main()