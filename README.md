file:///Users/nav/Desktop/test/pelmorex/Pelmorax-Guide.pdf
https://alerts.pelmorex.com/wp-content/uploads/2019/12/NAAD-System-Test-Message-Policy-EN.pdf
https://docs.oasis-open.org/emergency/cap/v1.2/CAP-v1.2.html

Modules/Libraries/Components
TCP listener
Timer (Heartbeats are sent once evey minute)
XML Parser
TTS, WAV file writer
RS232 Controller
Audio
Custom TTS library for toggling diffent quality of voices. ( I might not actually need this)

If an Alert occurs at 10:00, it will take precedence over the Heartbeat. 
The Heartbeat will be sent as well but after the Alert has been sent.
Duplicate alerts might be sent. So that should be checked for.

Max size of Alert = 5MB. Reset the connection if XML string surpasses this limit.

AGA may include attachments in two forms (including both simultaneously in the same message)
* Base64 Encoded Attachment(s)
* External Links to attachment(s) hosted
* Attachments may be invalid. They must be validated.

The Heartbeat message:
- is not digitally signed.
- contains a list of the latest (10) transmitted alert messages.
- a short-term repository (up to the last 48 hours) where missed alerts can be retrieved ,

### Test messages not intended for broadcast:
1. Heartbeat Message
2. Test message with Message status: Test
### Test messages intended for broadcast:
1. Test message with Message status: Actual 
2. Public awareness test messages.

To receive this feed, a TCP/IP socket application (client) must be used by the LMD and it must be able to connect to the following domains:
• streaming1.naad-adna.pelmorex.com (Oakville)
• streaming2.naad-adna.pelmorex.com (Montreal)
• TCP port is 8080 (for both sites)