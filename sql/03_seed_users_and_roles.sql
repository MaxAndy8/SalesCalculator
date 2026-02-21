-- Активуємо розширення (якщо версія PG < 13)
CREATE EXTENSION IF NOT EXISTS pgcrypto;

-- Початкове заповнення таблиць users та user_roles.
-- Паролі у відкритому вигляді (на даному етапі проєкту).
-- idrref формується як 16 байт (RFC 4122 UUID) для сумісності з QUuid.

-- Переконайтесь, що виконано 01 (users вже створена) та 02_user_roles.sql.
-- Повторний запуск не створює дублікатів за description.

-- Користувач admin з паролем admin
INSERT INTO public.users (
    idrref, version, marked, description, full_description, "show", password
)
SELECT decode(replace(gen_random_uuid()::text, '-', ''), 'hex'), 0, false,
       'admin', 'Адміністратор', true, convert_to('admin', 'UTF8')
WHERE NOT EXISTS (SELECT 1 FROM public.users WHERE description = 'admin');

-- Користувач з порожнім паролем (логін без пароля)
INSERT INTO public.users (
    idrref, version, marked, description, full_description, "show", password
)
SELECT decode(replace(gen_random_uuid()::text, '-', ''), 'hex'), 0, false,
       'guest', 'Гість (без пароля)', true, ''::bytea
WHERE NOT EXISTS (SELECT 1 FROM public.users WHERE description = 'guest');

-- Звичайний користувач
INSERT INTO public.users (
    idrref, version, marked, description, full_description, "show", password
)
SELECT decode(replace(gen_random_uuid()::text, '-', ''), 'hex'), 0, false,
       'user', 'Звичайний користувач', true, convert_to('user', 'UTF8')
WHERE NOT EXISTS (SELECT 1 FROM public.users WHERE description = 'user');

-- Ролі: прив'язка за description
INSERT INTO public.user_roles (user_idrref, role)
SELECT u.idrref, 'admin' FROM public.users u WHERE u.description = 'admin'
ON CONFLICT (user_idrref, role) DO NOTHING;

INSERT INTO public.user_roles (user_idrref, role)
SELECT u.idrref, 'user' FROM public.users u WHERE u.description = 'admin'
ON CONFLICT (user_idrref, role) DO NOTHING;

INSERT INTO public.user_roles (user_idrref, role)
SELECT u.idrref, 'user' FROM public.users u WHERE u.description = 'user'
ON CONFLICT (user_idrref, role) DO NOTHING;

INSERT INTO public.user_roles (user_idrref, role)
SELECT u.idrref, 'guest' FROM public.users u WHERE u.description = 'guest'
ON CONFLICT (user_idrref, role) DO NOTHING;
