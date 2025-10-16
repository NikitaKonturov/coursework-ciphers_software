#!/bin/bash

echo "🧹 Удаляем только основные большие файлы..."
echo "📊 Ожидаемая экономия: ~640 MB"

# Создаем backup
BACKUP_TAG="backup-before-cleanup-$(date +%Y%m%d-%H%M%S)"
echo "🔒 Создаем backup: $BACKUP_TAG"
git tag "$BACKUP_TAG"

echo ""
echo "📊 Текущий размер репозитория:"
git count-objects -v

echo ""
echo "1. 🗑️ Удаляем .pdb файлы (~450 MB)..."
git filter-branch --tree-filter 'find . -name "*.pdb" -type f -delete 2>/dev/null || true' --prune-empty -- --all

echo ""
echo "2. 🗑️ Удаляем большие .db файлы (~142 MB)..."
git filter-branch --tree-filter 'rm -f Dictionaries/words.db 2>/dev/null || true' --prune-empty -- --all

echo ""
echo "3. 🗑️ Удаляем файлы сборки (~48 MB)..."
git filter-branch --tree-filter 'rm -f build/Ciphers/Ciphers.pkg 2>/dev/null || true' --prune-empty -- --all

echo ""
echo "♻️ Очищаем историю Git..."
git reflog expire --expire=now --all
git gc --prune=now --aggressive

echo ""
echo "📊 Результат очистки:"
echo "===================="
git count-objects -v

echo ""
echo "🔍 Проверяем оставшиеся большие файлы:"
git rev-list --objects --all | git cat-file --batch-check='%(objecttype) %(objectname) %(objectsize) %(rest)' | sort -k3 -n | tail -10

echo ""
echo "✅ Очистка завершена!"
echo ""
echo "⚠️  Действия после очистки:"
echo "   1. Проверьте: git log --oneline -10"
echo "   2. Если всё хорошо, удалите backup: git tag -d $BACKUP_TAG"
echo "   3. Для отправки: git push --force-with-lease --all"
