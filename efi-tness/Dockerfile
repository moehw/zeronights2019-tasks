FROM kalilinux/kali-rolling

MAINTAINER konola

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install xinetd python3 qemu-system-x86 vim nano -y

RUN useradd -m efitness

ADD --chown=root:root ./deploy/ /home/efitness/deploy/
ADD --chown=root:root ./binary/ /home/efitness/binary/
ADD --chown=root:root xinetd /etc/xinetd.d/efitness

RUN chmod +x /home/efitness/deploy/run.sh
RUN chmod +x /home/efitness/deploy/run.py
RUN chmod +x /home/efitness/deploy/run-qemu.sh

RUN chmod 744 /tmp
RUN chmod -R 774 /var/tmp
RUN chmod -R 774 /dev
RUN chmod -R 777 /run
RUN chmod 1733 /tmp /var/tmp /dev/shm

RUN chown -R root:root /home/efitness/

WORKDIR /home/efitness/deploy

EXPOSE 30077/tcp

CMD ["/usr/sbin/xinetd", "-dontfork"]