import cv2


# Read an image
img = cv2.imread('F:\projects\CSEE\L_2025_Stm32H723ZGT6_Cmake\Lib\Scripts\Shinobu3.jpeg')

# Display it in a window
cv2.imshow('Image Window', img)
# Wait for a key press and then close the window
cv2.waitKey(0)
cv2.destroyAllWindows()