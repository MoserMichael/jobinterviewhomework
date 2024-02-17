package intuit.cdplayer.srvlet.rest;

import intuit.cdplayer.srvlet.data.PlayerTable;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.bind.annotation.PathVariable;

@RestController
public class CDPlayer {

    private final static String PlayerFileName = "player.csv";
    private final PlayerTable playerTable = new PlayerTable();

    CDPlayer(PlayerTable table) throws Exception {
        this.playerTable.read(PlayerFileName);
    }

    @GetMapping("/api/players")
    public String getAllPlayerIds() {
        var allPlayers = playerTable.getAllPlayers();
        return String.join(",", allPlayers);
    }

    @GetMapping("/api/players/{playerId}")
    public String getPlayerById(@PathVariable String playerId) {
        var playerData = playerTable.findPlayerById(playerId);
        if (playerData == null) {
            return String.format("Error: player %s does not exist", playerId);
        }
        var columns = playerTable.getColumnNames();
        assert columns.length == playerData.length;

        var res = new StringBuilder();
        for(int i=0; i<columns.length;++i) {
            var nameVal = String.format("%s%s=%s", i == 0 ? "" : ",", columns[i], playerData[i]);
            res.append(nameVal);
        }

        return res.toString();
    }

}