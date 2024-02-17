package intuit.cdplayer.srvlet.csvdata;

public interface CSVReaderInterface {
    void onRecord(String[] columnNames, String[] recordValues) throws Exception;
}
