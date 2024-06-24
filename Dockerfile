FROM mathworks/matlab:r2023b

USER root
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends software-properties-common curl git unzip build-essential zlib1g-dev libncurses5-dev libgdm-dev libnss3-dev libssl-dev libreadline-dev libffi-dev libbz2-dev libsqlite3-dev gnuplot
RUN add-apt-repository -y "ppa:deadsnakes/ppa" && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y python3.8 python3.8-venv python3-venv
#USER matlab

RUN cd $HOME && git clone https://github.com/choshina/EfficientCausationMonitor.git

#WORKDIR /root/EfficientCausationMonitor

#ENTRYPOINT [ "/bin/bash"]
CMD ["matlab", "-r", "cd /root/EfficientCausationMonitor/;addpath /root/EfficientCausationMonitor/Online/bin/"]
#CMD ["matlab", "-r", "addpath /root/EfficientCausationMonitor/Online/bin/"]
#ENTRYPOINT [ "/bin/run.sh", "-shell" ]
