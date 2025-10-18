FROM archlinux:base-devel


#Обновление системы
RUN pacman -Syu --noconfirm

RUN pacman -S --noconfirm python python-pip

WORKDIR/app
COPY ..

RUN python -m venv/app/venv
	
RUN/app/venv/bin/pip install -r requirements.txt

CMD["/app/venv/bin/python", "app.py"]


