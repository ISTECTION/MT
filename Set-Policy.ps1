Set-ExecutionPolicy Unrestricted

# Вернуть политику можно прописав данную команду
# Set-ExecutionPolicy Restricted


# 1. Ограниченный (Restricted)              — значение по умолчанию. Блокируется выполнение любых скриптов и разрешается работа интерактивных команд.
# 2. Все подписанные (All Signed)           — разрешено выполнение скриптов, имеющих цифровую подпись.
# 3. Удаленные подписанные (Remote Signed)  — локальные скрипты работают без подписи. Все скачанные скрипты должны иметь цифровую подпись.
# 4. Неограниченный (Unrestricted)          — разрешено выполнение любых скриптов. При запуске не подписанного скрипта, который был загружен из Интернета, программа может потребовать подтверждение.
# 5. Обходной (Bypass)                      — ничего не блокируется, никакие предупреждения и запросы не появляются