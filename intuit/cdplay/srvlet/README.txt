Intuit homework
---------------

Note: this archive contains .bat files. that's why i could not send it via mail.

I renamed files with extension .bat to extension .no-bat - for the purpose of sending the archive.

Implements 
----------

GET request for /api/players 

Returns text - list of all player ids, comma delimited

GET request for /api/players/{playerId}

Returns text = comma delimited name=value pairs of the record for specific {playerId}

Files
-----


build.bat - builds docker image intuit-homework:latest that hosts the server
run.bat   - runs the docker image on port 8080
test.bat  - sends some http request to the service via curl

Implementation
--------------

Reads the file player.csv (hardcoded name) from file system into memory.

Builds an in-memory table representation (class intuit.cdplayer.srvlet.data.PlayerTable)  This class has a in memory index (map of string to record data), and holds the array list of all player ids.

The servlet is implemented in class    intuit.cdplayer.srvlet.rest.CDPlayer

Utility classes for reading a CSV file intuit.cdplayer.srvlet.csvdata.CSVReader and intuit.cdplayer.srvlet.csvdata.CSVReaderInterface

Note: We don't need to deal with synchronization, as the data is being treated in a read only matter, the CSV file is being read into memory during initialization, and synchronization is not required.

Possible improvements
---------------------

use special exception classes rather than using Exception

location of CSV file is hard coded, be more flexible about the location of the CSV file.    

