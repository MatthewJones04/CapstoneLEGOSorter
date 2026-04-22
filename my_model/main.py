import cv2

from database import (
    setup_database,
    populate_tables_from_folders
)
from Routing import assign_piece
# from yolo_detect import initidalize_serial, run_inference

if __name__ == "__main__":
    # Step 1: Setup database and tables
    setup_database()
    # initialize_serial()  # Commented out for testing without hardware

    # Step 2: Testing interface
    print("Testing Interface:")
    print("Press 't' to test routing with a piece")
    print("Press 'q' to quit")

    while True:
        choice = input("Enter choice: ").strip().lower()
        if choice == 't':
            piece_id = "63782"
            color = "black"
            result = assign_piece(piece_id, color)
            bin_code, tray_number = result
            print(f"Bin code for {piece_id} ({color}): {bin_code}, Tray Number: {tray_number}")
        
        elif choice == 'q':
            print("Exiting...")
            break
        
        else:
            print("Invalid choice. Try again.")

        
        