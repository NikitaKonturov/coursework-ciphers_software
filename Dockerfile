FROM archlinux:base-devel


#Обновление системы
RUN pacman -Syu --noconfirm

RUN pacman -S --noconfirm python python-pip

WORKDIR /app
COPY . .

RUN python -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

RUN pip install -r requirements.txt
	
#RUN pip install -r requirements.txt

EXPOSE 8000

CMD ["python", "app.py"]


