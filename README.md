# SalesCalculator

Облікова система продажів — десктопний застосунок на Qt 6 (C++20) з архітектурою за принципами Clean Architecture.

## Вимоги

- **CMake** 3.21+
- **Qt** 6.5+ (Core, Widgets, Sql, LinguistTools)
- **C++20** (компілятор з підтримкою C++20)
- **PostgreSQL** (для зберігання даних та авторизації)

## Збірка

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Або з генератором Ninja:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Виконуваний файл: `build/SalesCalculator` (або `build/Release/SalesCalculator.exe` на Windows).

## Запуск

1. Запустіть застосунок.
2. Виберіть базу даних (або додайте нове підключення) у діалозі вибору БД.
3. Увійдіть під користувачем (логін та пароль з БД).
4. Після успішної авторизації відкривається головне вікно.

## База даних (PostgreSQL)

Таблиці та початкові дані описані в папці `sql/`. Виконайте скрипти по черзі:

1. **`sql/01_users.sql`** — таблиця користувачів (`users`).
2. **`sql/02_user_roles.sql`** — таблиця ролей (`user_roles`).
3. **`sql/03_seed_users_and_roles.sql`** — початкові користувачі та ролі (admin, user, guest).

Паролі на поточному етапі зберігаються у відкритому вигляді; підтримується вхід з порожнім паролем.

## Структура проєкту

Проєкт організований за шарами Clean Architecture:

| Шар | Каталог | Призначення |
|-----|---------|-------------|
| **Domain** | `src/domain` | Бізнес-правила, сутності (поки майже не використовується) |
| **Application** | `src/application` | Порты (інтерфейси), DTO, сценарії (наприклад, `IAuthService`, `AuthenticatedUser`) |
| **Infrastructure** | `src/infrastructure` | Реалізації портів (БД, репозиторії): `SqlAuthService`, `DbConnectionProvider` |
| **Core** | `src/core` | Спільне ядро: логер, утиліти |
| **UI** | `src/ui` | Вікна, діалоги (MainWindow, AuthDialog, SelectDatabaseDialog) |
| **App** | `src/app` | Точка входу, composition root: `main()`, збирання залежностей |

Додатково: `src/ui_resources` (ресурси Qt), `src/ui_i18n` (переклади).

Залежності йдуть всередину: UI та Infrastructure залежать від Application; UI не залежить від Infrastructure — реалізації підставляються в точці збирання (App).

## Тести

Опційно включити тести при конфігурації:

```bash
cmake -B build -DSC_ENABLE_TESTS=ON
cmake --build build
ctest --test-dir build
```

## Ліцензія

Проєкт поширюється під вільною ліцензією: код можна використовувати, змінювати та поширювати. При використанні або поширенні прохання згадувати автора.

**Автор:** Andy
