# database.py
import os
import re
import sqlite3
from datetime import datetime

DB_PATH = "lego_tables.db"
TOTAL_TABLES = 16


# ==========================================================
# INITIALIZATION
# ==========================================================

def initialize_database():
    # If the database file exists, remove it so we replace it fresh.
    if os.path.exists(DB_PATH):
        os.remove(DB_PATH)
        print("Removed existing database file.")

    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    for i in range(1, TOTAL_TABLES + 1):
        cursor.execute(f"""
            CREATE TABLE IF NOT EXISTS table_{i} (
                primary_id TEXT NOT NULL,
                name TEXT NOT NULL,
                color TEXT NOT NULL,
                quantity INTEGER NOT NULL,
                bin_number INTEGER NOT NULL,
                PRIMARY KEY (primary_id, color)
            )
        """)

    conn.commit()
    conn.close()


# ==========================================================
# DATABASE OPERATIONS
# ==========================================================
# returns quantity of piece in table, if color sensitive, also checks color
def get_quantity(table_name, piece_id, color=None):
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    if color:
        color = color.lower()
        cursor.execute(f"""
            SELECT quantity FROM {table_name}
            WHERE primary_id=? AND color=?
        """, (piece_id, color))
    else:
        cursor.execute(f"""
            SELECT quantity FROM {table_name}
            WHERE primary_id=?
        """, (piece_id,))

    result = cursor.fetchone()
    conn.close()

    return result[0] if result else None

# decrement quantity of piece in table, if color sensitive, also checks color
def decrement_quantity(table_name, piece_id, color=None):
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    if color:
        color = color.lower()
        cursor.execute(f"""
            UPDATE {table_name}
            SET quantity = quantity - 1
            WHERE primary_id=? AND color=?
        """, (piece_id, color))
    else:
        cursor.execute(f"""
            UPDATE {table_name}
            SET quantity = quantity - 1
            WHERE primary_id=?
        """, (piece_id,))

    conn.commit()
    conn.close()


# gets list of tables in priority order
def get_table_priority_list():
    return [f"table_{i}" for i in range(1, TOTAL_TABLES + 1)]


# gets bin_number for a specific piece_id and color combination
def get_bin_number(piece_id: str, color: str = None) -> int:
    """
    Returns the bin_number for a piece_id and optional color.
    Searches all tables in priority order and returns the first match.
    """
    for table_name in get_table_priority_list():
        conn = sqlite3.connect(DB_PATH)
        cursor = conn.cursor()

        if color:
            color = color.lower()
            cursor.execute(f"""
                SELECT bin_number FROM {table_name}
                WHERE primary_id=? AND color=?
                LIMIT 1
            """, (piece_id, color))
        else:
            cursor.execute(f"""
                SELECT bin_number FROM {table_name}
                WHERE primary_id=?
                LIMIT 1
            """, (piece_id,))

        result = cursor.fetchone()
        conn.close()

        if result:
            return result[0]

    return None


# ==========================================================
# FOLDER/FILE LOADER
# ==========================================================

def extract_inserts_from_path(path):
    """
    Read all .txt data from a file or directory and return parsed rows.
    Expected row format:
        ('id','name','color',quantity,bin_number)
    """
    if os.path.isfile(path):
        files = [path]
    elif os.path.isdir(path):
        files = [
            os.path.join(path, f)
            for f in os.listdir(path)
            if f.lower().endswith(".txt")
        ]
    else:
        raise FileNotFoundError(f"Path not found: {path}")

    extracted_data = []
    row_pattern = re.compile(
        r"\(\s*['\"](\d+)['\"]\s*,\s*['\"](.*?)['\"]\s*,\s*['\"](.*?)['\"]\s*,\s*(\d+)\s*,\s*(\d+)\s*\)",
        re.DOTALL
    )

    for file_path in files:
        with open(file_path, "r", encoding="utf-8") as file:
            content = file.read()

        for piece_id, name, color, quantity, bin_number in row_pattern.findall(content):
            extracted_data.append(
                (
                    piece_id.strip(),
                    name.strip(),
                    color.strip().lower(),
                    int(quantity),
                    int(bin_number),
                )
            )

    return extracted_data


def populate_tables_from_folders(folder_a: str,):
    """
    Reads data from one or two folder/file paths.
    If a path is a file it reads that file; if it is a folder it reads all .txt files.
    Data is inserted into tables based on the parsed bin_number.
    """
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    extracted_data = extract_inserts_from_path(folder_a)

    print(f"\n[INFO] Total inserts extracted: {len(extracted_data)}")

    # Clear all tables before populating
    for table_number in range(1, TOTAL_TABLES + 1):
        cursor.execute(f"DELETE FROM table_{table_number}")

    # Track inserts per table for verification
    table_counts = {i: 0 for i in range(1, TOTAL_TABLES + 1)}
    
    # Insert all extracted data into appropriate tables
    for piece_id, name, color, quantity, bin_number in extracted_data:
        
        for table in range(1, TOTAL_TABLES + 1):
            cursor.execute(
                f"""
                INSERT INTO table_{table}
                (primary_id, name, color, quantity, bin_number)
                VALUES (?, ?, ?, ?, ?)
                """,
                (piece_id, name, color, quantity, bin_number),
            )
            table_counts[table] += 1
        # if 1 <= bin_number <= TOTAL_TABLES:
        #     cursor.execute(
        #         f"""
        #         INSERT INTO table_{bin_number}
        #         (primary_id, name, color, quantity, bin_number)
        #         VALUES (?, ?, ?, ?, ?)
        #         """,
        #         (piece_id, name, color, quantity, bin_number),
        #     )
        #     table_counts[bin_number] += 1
        # else:
        #     print(f"[ERROR] Skipping row with invalid bin_number {bin_number}: {piece_id} {name}")

    conn.commit()
    conn.close()

    # Print statistics
    print("\n[SUCCESS] All tables successfully populated.")
    print("[STATISTICS] Inserts per table:")
    for bin_num in range(1, TOTAL_TABLES + 1):
        count = table_counts[bin_num]
        if count > 0:
            print(f"  table_{bin_num}: {count} inserts")


def verify_database():
    """Verify that all tables contain data"""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    print("\n[VERIFICATION] Current row counts per table:")
    total_rows = 0
    for table_num in range(1, TOTAL_TABLES + 1):
        cursor.execute(f"SELECT COUNT(*) FROM table_{table_num}")
        count = cursor.fetchone()[0]
        total_rows += count
        if count > 0:
            print(f"  table_{table_num}: {count} rows")
    
    conn.close()
    print(f"\n[TOTAL] {total_rows} rows across all tables")
    return total_rows


# ==========================================================
# MAIN SETUP
# ==========================================================

def setup_database():
    """Initialize database and populate all tables with ALL inserts from Insert.txt"""
    print("[INIT] Initializing database...")
    initialize_database()
    print("[LOAD] Loading all inserts from Insert.txt...")
    populate_tables_from_folders("Insert.txt")
    verify_database()


