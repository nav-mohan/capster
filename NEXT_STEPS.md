Figure out how to use the `boost:asio::strand` when parallelizing `Client1` and `Client2`. They will eventually share these resources :
1. the DTR pin
2. the output audio to hardware (kinda like sharing stdout)
so we need to check and ensure that both clients are receiving the same alert before using one of them. There might be an ID number or a Signature.

Figure out a way to buffer the XML. 
The socket will be writing. The regexParser will be reading. 
Just use a std::vector<char> for the buffer so you can expand it as you wish. 
Run the regex-test at the end of each `do_read()` operation. 
If the regex test returns a match then splice the string into `matched` and `unmatched`


How do I remove a section of the string from the fullBuffer_ if it satisfies regex?

I dont know if CoreAudio can play an Mpeg3 file. I might have to decode it to WAV using libmp3lame. And then use CoreAudio or PortAudio to play the WAV file. 

Dont know what's going wrong here
For some reason I suddenly seem to be getting a bad-file-descriptor when running cancelling/shutting-down/closing the tcpSocket. This didnt happen until now and not sure whats causing this. 
bad file-descriptor usually means that the socket closed prematurely. stackoverflow suggested that it might be due to socket exiting it's scope. but if the socket is a member variable then it shouldnt go out of scope.
Ok figured out what the problem was.
Something to do with the placement of the fullBuffer_.data() within the async_receive() function. It had to be within the if(!ec) condition scope and not within the bigger lambda scope.
ok i think i might have funally figured out the loop
SO DONT FUCKING TOUCH IT ANYMORE! Just workon the regex thing and move on to the xml

## BUGS
* Theres a memory leak somewhere. 
    - `download_file()` . Updating curl did not _entirely_ fix the issue. 
    - `decode_and_write()`  
    - `fullBuffer_`, `matches_`
* Eraing the fullBuffer_ has some problem. Not sure what it is. but it gives a std::out_of_range::basic_string error if the server sends out the xml stream rapidly. it might be that both clients are writing to the same std::map<>fullBuffer_ and there might be some problems with resizing the strings in the specific memory areas. 
ok. it migth be something specific to Heartbeat1.xml. Not sure what. Could just be the formatting of it. If i use Heartbeat2,3,4.xml or event copy one of them into Heartbeat1.xml the erase process works fine even without any sleep delays on the server.

## FEATURES REQUIRED
* verify signatures (requires HTTPS, so BoostClient with SSL)
* Retrieve missed alerts 
* Perform TTS for _Urgent alerts_ without audio/mpeg resources
* Set a broadcast area rather than a station location - check for intersection of two polygons rather than whether polygon contains point
* Build a GUI


Once I confirm that there are no more "Missing Node" errirs I can remove the try-catch block

Audio could be in Mpeg, WMA, or WAV. So I'll have to check the mimeType 
    "audio/wav" or "audio/x-wav".
    audio/x-ms-wma
    audio/mpeg

The audio could either be Base64 encoded or in a remote URL
check if mimetype.substr(0,5) == "audio"
If Base64 Encoded, then there is a derefUri
If not Base64 encoded then the derefuri is missing and the uri will contain a remote https url
The total size of all attachements will be less than 800kb
