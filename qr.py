import qrcode

# Function to create a QR code for a URL
def generate_qr_code(url, message, file_name):
    data = f"{url}\n\n{message}"
    qr = qrcode.QRCode(
        version=1,
        error_correction=qrcode.constants.ERROR_CORRECT_L,
        box_size=10,
        border=4,
    )
    
    qr.add_data(data)
    qr.make(fit=True)

    img = qr.make_image(fill_color="black", back_color="white")

    img.save(file_name)

if __name__ == "__main__":
    url = "http://192.168.1.1/"
    message = "Scan this QR code and follow the instructions to add a bookmark for easy access."

    file_name = "qr_code_with_message.png"

    generate_qr_code(url, message, file_name)
    print(f"QR code for {url} has been generated with a message and saved as {file_name}.")
