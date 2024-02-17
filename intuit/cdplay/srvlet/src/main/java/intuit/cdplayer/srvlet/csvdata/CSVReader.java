package intuit.cdplayer.srvlet.csvdata;
import java.io.*;

public class    CSVReader {

    public void readFile(String fileName, CSVReaderInterface readerInterface) throws Exception {
        BufferedReader reader = null;
        int lineNumber = 0;
        String [] columnNames = null;

        try {
            reader = new BufferedReader(new FileReader(fileName));
            String line = reader.readLine();

            while (line != null) {
                var values = line.split(",", -1);

                line = reader.readLine();
                if (lineNumber != 0) {
                    if (values.length != columnNames.length) {
                        //var msg = String.format("Expected %d column values, got %d = %s", columnNames.length, values.length, String.join(" # ", values));
                        var msg = String.format("Expected %d column values, got %d", columnNames.length, values.length);
                        throw new Exception(msg);
                    }
                    readerInterface.onRecord( columnNames, values );
                } else {
                    columnNames = values;
                }
                lineNumber += 1;
            }
        } catch (Exception e) {
            var msg = String.format("CVS error at line %d : %s", lineNumber, e.toString());
            throw new Exception(msg);
        } finally {
            if (reader != null) {
                reader.close();
            }
        }
    }
}
