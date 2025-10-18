#!/bin/bash
BRANCHES=("viginer_sql" "simple_substitution_cipher" "jeferson-bazeri_cipher" "Release" "Plaifer_cipher" "Homophone_cipher" "Hill_cipher" "Gamut_cipher" "Cardan" "API_cpp_py" "ADFGVX_cipher")
NEW_SUFFIX="_recent"

INITIAL_BRANCH=$(git branch --show-current)

for BRANCH in "${BRANCHES[@]}"; do
    echo "🔄 Processing: $BRANCH"
    
    # Проверяем достаточно ли коммитов
    COMMIT_COUNT=$(git rev-list --count "$BRANCH")
    if [ "$COMMIT_COUNT" -lt 3 ]; then
        echo "❌ Only $COMMIT_COUNT commits in $BRANCH (need 3)"
        continue
    fi
    
    NEW_BRANCH="${BRANCH}${NEW_SUFFIX}"
    
    # Переключаемся на main и удаляем старую ветку
    git checkout main
    git branch -D "$NEW_BRANCH" 2>/dev/null
    git push origin --delete "$NEW_BRANCH" 2>/dev/null
    
    # Получаем 3 последних коммита
    COMMIT1=$(git rev-parse "$BRANCH")      # самый новый
    COMMIT2=$(git rev-parse "$BRANCH"~1)    # средний
    COMMIT3=$(git rev-parse "$BRANCH"~2)    # самый старый
    
    echo "Last 3 commits from $BRANCH:"
    git log --oneline "$BRANCH" -3
    
    # СОЗДАЕМ ПОЛНОСТЬЮ НОВУЮ ВЕТКУ БЕЗ ИСТОРИИ (orphan)
    git checkout --orphan "$NEW_BRANCH"
    git reset --hard
    
    # Копируем файлы из САМОГО СТАРОГО коммита
    git checkout "$COMMIT3" -- .
    git add .
    git commit -m "$(git log --format=%B -n 1 "$COMMIT3")"
    
    # Копируем изменения из СРЕДНЕГО коммита
    git checkout "$COMMIT2" -- .
    git add .
    git commit -m "$(git log --format=%B -n 1 "$COMMIT2")"
    
    # Копируем изменения из ПОСЛЕДНЕГО коммита
    git checkout "$COMMIT1" -- .
    git add .
    git commit -m "$(git log --format=%B -n 1 "$COMMIT1")"
    
    # Проверяем результат
    FINAL_COUNT=$(git log --oneline | wc -l)
    echo "Commits in new branch: $FINAL_COUNT"
    
    # Пушим
    git push -u origin "$NEW_BRANCH" --force
    
    echo "✅ Created: $NEW_BRANCH with $FINAL_COUNT commits"
    echo "---"
done

git checkout "$INITIAL_BRANCH"
echo "🎉 Done!"