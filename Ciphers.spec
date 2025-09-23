# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['app.py'],
    pathex=['C:/Development/course_work/coursework-ciphers_software'],
    binaries=[
        ('C:/Development/course_work/coursework-ciphers_software/ciphers_api_module/cpp_exceptions.pyd', 'ciphers_api_module/'),
        ('C:/Development/course_work/coursework-ciphers_software/ciphers_api_module/telegrams_cutter.pyd', 'ciphers_api_module/')
    ],
    datas=[
        ('templates', 'templates'),  # Шаблоны для Jinja2
        ('static', 'static'),  # Статические файлы
        ('settings/config.py', 'settings'),  # Конфиги
        ("Ciphers/", "Ciphers/"), # Шифры
        ("Dictionaries/", "Dictionaries/"), # Словари
    ],
    hiddenimports=[
        'ciphers_api_module',
        'ciphers_api_module.ciphers_api_module',
        'ciphers_api_module.requestsClass.requestToEncript',
        'exception_handlers'
        'cpp_exceptions', 
        'ciphers_api_module.telegrams_cutter'  
  
    ],
    hookspath=[],
    runtime_hooks=[],
    excludes=[],
    noarchive=False
)

a.datas += [('logo.ico', 'C:\\Development\\course_work\\coursework-ciphers_software\\logo.ico', 'DATA')]
pyz = PYZ(a.pure, a.zipped_data)


exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.datas,
    [],
    name='Ciphers',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    icon='logo.ico' 
)
