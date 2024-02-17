package intuit.cdplayer.srvlet;

import intuit.cdplayer.srvlet.csvdata.*;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;

import java.util.ArrayList;

@SpringBootTest
class SrvletApplicationTests {

    static class TestReceiver implements CSVReaderInterface {

        public String[] columnNames;
        public java.util.List<String[]> records;
        public TestReceiver() {
            this.records = new ArrayList<String[]>();
        }
       public void onRecord(String[] columnNames, String[] recordValues) throws Exception {
           this.columnNames = columnNames;

       }

    }

    static final int COLUMN_COUNT_EX1 = 5;
    static final int ROW_COUNT_EX1 = 3;

    @Test
    void contextLoads() {
        var receiver = new TestReceiver();
        var reader = new CSVReader();

        reader.readFile("example.csv");

        assertEquals(receiver.columnNames.length == COLUMN_COUNT_EX1);
        assertEquals(receiver.records.length == ROW_COUNT_EX1);
    }

}
