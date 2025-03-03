# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['app.py'],
    pathex=['C:/Development/course_work/coursework-ciphers_software'],
    binaries=[],
    datas=[
        ('templates', 'templates'),  # Шаблоны для Jinja2
        ('static', 'static'),  # Статические файлы
        ('settings/config.py', 'settings'),  # Конфиги
        ("Ciphers/", "Ciphers/") # Шифры
    ],
    hiddenimports=[
        'ciphers_api_module',
        'ciphers_api_module.ciphers_api_module',
        'ciphers_api_module.requestsClass.requestToEncript',
        'exception_handlers'
    ],
    hookspath=[],
    runtime_hooks=[],
    excludes=[],
    noarchive=False
)

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
)
