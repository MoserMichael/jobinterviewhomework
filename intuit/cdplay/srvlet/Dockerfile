FROM gradle:8.5.0-jdk21-alpine AS build
COPY --chown=gradle:gradle . /home/gradle/src

WORKDIR /home/gradle/src
RUN gradle build --no-daemon 

FROM openjdk:21-oracle

EXPOSE 8080

RUN mkdir /app

COPY --from=build /home/gradle/src/player.csv  /player.csv
COPY --from=build /home/gradle/src/build/libs/*.jar /app/spring-boot-application.jar

# ENTRYPOINT ["java", "-XX:+UnlockExperimentalVMOptions", "-XX:+UseCGroupMemoryLimitForHeap", "-Djava.security.egd=file:/dev/./urandom","-jar","/app/spring-boot-application.jar"]
ENTRYPOINT ["java", "-Djava.security.egd=file:/dev/./urandom","-jar","/app/spring-boot-application.jar"]

