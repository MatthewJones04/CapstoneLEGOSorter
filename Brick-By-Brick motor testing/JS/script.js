let port;
let writer;

// Debug logger
function log(msg) {
    const consoleDiv = document.getElementById("console");
    consoleDiv.innerHTML += msg + "<br>";
    consoleDiv.scrollTop = consoleDiv.scrollHeight;
}

// CONNECT
async function connectArduino() {
    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });

        writer = port.writable.getWriter();

        log("Connected to Arduino");
    } catch (err) {
        log("Connection error: " + err);
    }
}

// DISCONNECT
async function disconnectArduino() {
    try {
        if (writer) writer.releaseLock();
        if (port) await port.close();

        log("Disconnected from Arduino");
    } catch (err) {
        log("Disconnect error: " + err);
    }
}

// SEND MESSAGE
async function sendMessage() {
    const msg = document.getElementById("messageInput").value;

    // Validate code input (allow IN, ID, LI, B, 1T, 0T formats)
    if (!/^(IN[01]|ID[01]|LI1|B\d{2}|[01]T\d{2})$/.test(msg)) {
        log("Error: Valid codes include IN0/IN1, ID0/ID1, LI1, B00-B13, 1T00-1T13, 0T00-0T13");
        return;
    }

    try {
        const data = new TextEncoder().encode(msg + "\n");
        await writer.write(data);
        log("Sent: " + msg);
    } catch (err) {
        log("Send error: " + err);
    }
}

// HELP POPUP
function openHelp() {
    document.getElementById("helpPopup").style.display = "block";
}
function closeHelp() {
    document.getElementById("helpPopup").style.display = "none";
}

// MENU POPUP
function openMenu() {
    document.getElementById("menuPopup").style.display = "block";
}
function closeMenu() {
    document.getElementById("menuPopup").style.display = "none";
}

// MENU BUTTON ACTIONS
function menuAction(btn) {
    // Set the message input field and send the message
    document.getElementById("messageInput").value = btn;
    sendMessage();
    closeMenu();
}