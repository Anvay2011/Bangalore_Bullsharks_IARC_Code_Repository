import cv2  # Import OpenCV library for image processing
import numpy as np  # Import NumPy for numerical operations

proportion = 0  # Initialize proportion variable for displacement calculation
p_factor = 1  # Scaling factor for proportional displacement

# Start capturing video from the default webcam (device 0)
cap = cv2.VideoCapture(0)

while True:
    # Read a frame from the webcam
    ret, frame = cap.read()

    # If frame is not read properly, exit the loop
    if not ret:
        break

    # Convert the frame to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Get the dimensions of the frame: height, width
    height, width, channels = frame.shape

    # Draw a small red circle at the center of the frame for reference
    cv2.circle(frame, (width // 2, height // 2), 5, (0, 0, 255), 1)

    # Display the coordinates of the frame center on the screen
    cv2.putText(frame, f"Frame Center: ({width // 2}, {height // 2})",
                (width // 2 + 10, height // 2 - 10), cv2.FONT_HERSHEY_SIMPLEX,
                0.5, (0, 0, 255), 1)

    # Define the threshold range for detecting black color in grayscale
    lower_black = 0  # Lower bound for black (grayscale)
    upper_black = 50  # Upper bound for black (grayscale)

    # Create a binary mask where black pixels in the grayscale frame turn white (255) and others become black (0)
    mask = cv2.inRange(gray, lower_black, upper_black)

    # Create a new frame with all black pixels
    black_frame = np.zeros_like(frame)

    # Set the detected black pixels to white in the new frame
    black_frame[mask > 0] = [255, 255, 255]  # Highlight detected black areas in white

    # Compute image moments of the mask to determine the centroid
    moments = cv2.moments(mask)

    # Check if the detected area is non-zero to avoid division by zero error
    if moments["m00"] != 0:
        # Compute the centroid (center of gravity) of the detected black region
        cx = int(moments["m10"] / moments["m00"])  # X-coordinate of centroid
        cy = int(moments["m01"] / moments["m00"])  # Y-coordinate of centroid

        # Calculate the displacement of centroid from the frame center
        proportion = (cx - width // 2) * p_factor

        # Determine the position of centroid relative to the frame center
        if cx < width // 2:
            print('COG is on the left')  # Print if the centroid is on the left
        elif cx > width // 2:
            print('COG is on the right')  # Print if the centroid is on the right

        # Draw a red dot at the centroid of the detected black region
        cv2.circle(black_frame, (cx, cy), 5, (0, 0, 255), -1)

        # Display the centroid coordinates on the frame
        cv2.putText(black_frame, f"Center: ({cx}, {cy})", (cx + 10, cy - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1)

        # Draw a red circle at the frame's center for reference
        cv2.circle(black_frame, (width // 2, height // 2), 5, (0, 0, 255), 1)

        # Display the frame center coordinates on the black frame
        cv2.putText(black_frame, f"Frame Center: ({width // 2}, {height // 2})",
                    (width // 2 + 10, height // 2 - 10), cv2.FONT_HERSHEY_SIMPLEX,  0.5, (0, 0, 255), 1)

    # Display the original webcam feed
    cv2.imshow("Original", frame)

    # Display the processed black-and-white detection frame
    cv2.imshow("Black Line Detection", black_frame)

    # Wait for the user to press 'q' to exit the loop
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the webcam resource and close all OpenCV windows
cap.release()
cv2.destroyAllWindows()
