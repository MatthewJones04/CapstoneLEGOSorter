import threading

#import from database file
from database import (
    get_quantity,
    decrement_quantity,
    get_table_priority_list,
    get_bin_number
)

# ==========================================================
# CONFIGURATION
# ==========================================================

# Pieces that share ID but require strict color matching
COLOR_STRICT_IDS = {
# Lego MindStorms kit
    "41482",
    "32526",
# Spike Prime Kit
    "49283",
    "39789",
    "39793"
}

REJECT_BIN = "B00"


# ==========================================================
# HELPER FUNCTIONS
# ==========================================================

def table_to_bin(table_name: str) -> str:
    """
    Converts table_3 -> B03
    """
    number = int(table_name.split("_")[1])
    return f"B{number:02d}"


def is_color_sensitive(piece_id: str) -> bool:
    return piece_id in COLOR_STRICT_IDS

# ==========================================================
# CORE FUNCTION
# ==========================================================

def assign_piece(piece_id: str, color: str = "red") -> tuple:
    """
    Searches tables in priority order.
    If piece required, decrement quantity and return bin command and tray number.
    Returns (bin_code, tray_number) or ("B00", 0) if not found.
    """

    for table_name in get_table_priority_list():

        # --------------------------------------------------
        # UNIQUE COLOR LOGIC
        # --------------------------------------------------
        #checks if the piece has ID containing different colors, if so, it will check the quantity of that specific color in the table
        if is_color_sensitive(piece_id):
            quantity = get_quantity(table_name, piece_id, color)
        else:
            quantity = get_quantity(table_name, piece_id)

        #decrements quantity if piece is found and returns bin code and tray number, otherwise continues searching tables
        if quantity and quantity > 0:

            if is_color_sensitive(piece_id):
                decrement_quantity(table_name, piece_id, color)
                bin_number = get_bin_number(piece_id, color)
            else:
                decrement_quantity(table_name, piece_id)
                bin_number = get_bin_number(piece_id)

            bin_code = f"B{bin_number:02d}"
            tray_number = int(table_name.split("_")[1])
            return (bin_code, tray_number)

    return (REJECT_BIN, 0)

# ==========================================================
# SIMULATION LOOP (FOR TESTING WITHOUT HARDWARE)
# ==========================================================

def simulate_input():
    """
    Simulates incoming piece detections.
    Replace later with camera or sensor trigger.
    """
    while True:
        piece_id = input("Enter piece ID: ")
        color = input("Enter color (default red): ") or "red"

        bin_code = assign_piece(piece_id, color)

        print(f"Arduino Command -> {bin_code}\n")
