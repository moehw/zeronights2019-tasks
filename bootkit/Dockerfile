FROM ubuntu:19.04

MAINTAINER konola

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install xinetd -y

RUN useradd -m bootkit

ADD --chown=root:root ./service/wrapper/wrapper /home/bootkit/
ADD --chown=root:root ./service/disk/disk.bin /home/bootkit/
ADD --chown=root:root ./service/run.sh /home/bootkit/
ADD --chown=root:root xinetd /etc/xinetd.d/bootkit

RUN chmod +x /home/bootkit/wrapper
RUN chmod +x /home/bootkit/run.sh

RUN chmod 744 /tmp
RUN chmod -R 774 /var/tmp
RUN chmod -R 774 /dev
RUN chmod -R 774 /run
RUN chmod 1733 /tmp /var/tmp /dev/shm

RUN chown -R root:root /home/bootkit/

EXPOSE 30087/tcp

CMD ["/usr/sbin/xinetd", "-dontfork"]