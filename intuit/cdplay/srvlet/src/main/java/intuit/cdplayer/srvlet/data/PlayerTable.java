        package intuit.cdplayer.srvlet.data;
import  intuit.cdplayer.srvlet.csvdata.*;
import java.util.*;

// Note: access is currently strictly read-only.
// Need to deal with mt synchronization, if data is modified during runtime.
public class PlayerTable implements CSVReaderInterface {


    final private List<String> playerIds = new ArrayList<String>();
    final private Map<String, String[]>  index = new HashMap<String, String[]>();
    private String[] columnNames = null;

    public List<String> getAllPlayers() {
        return this.playerIds;
    }

    public String[] findPlayerById(String playerId) {
        return this.index.get(playerId);
    }

    public String[] getColumnNames() {
        return this.columnNames;
    }

    public void read(String fileName) throws Exception {
        var reader = new CSVReader();
        reader.readFile(fileName, this);
    }

    public void onRecord(String[] colNames, String[] recordValues) throws Exception {

        this.columnNames = colNames;

        if (colNames.length == 0) {
            throw new Exception("Missing columns");
        }
        if (recordValues.length == 0) {
            throw new Exception("Missing record");
        }
        if (!columnNames[0].equals("playerID")) {
            throw new Exception("No playerID column in player definition");
        }
        var playerId = recordValues[0];
        if (this.index.get(playerId) != null) {
            var msg = String.format("player with id %s defined twice", playerId);
            throw new Exception(playerId);
        }

        this.playerIds.add(playerId);
        this.index.put(playerId, recordValues);
    }
}
