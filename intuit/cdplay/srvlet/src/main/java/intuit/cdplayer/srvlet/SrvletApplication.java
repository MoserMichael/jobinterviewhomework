package intuit.cdplayer.srvlet;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class SrvletApplication {


    public static void main(String[] args) {
        System.out.println("Working Directory = " + System.getProperty("user.dir"));
        System.out.println("Java home = " + System.getProperty("java.home"));

        SpringApplication.run(SrvletApplication.class, args);
    }

}
