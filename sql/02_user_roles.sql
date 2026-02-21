-- Таблиця ролей користувачів (зв'язок users ↔ ролі).
-- Один користувач може мати кілька ролей (наприклад admin, user, manager).

-- DROP TABLE IF EXISTS public.user_roles;

CREATE TABLE IF NOT EXISTS public.user_roles
(
    user_idrref bytea NOT NULL,
    role character varying(64) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT user_roles_pkey PRIMARY KEY (user_idrref, role),
    CONSTRAINT user_roles_user_fkey FOREIGN KEY (user_idrref)
        REFERENCES public.users (idrref) ON DELETE CASCADE
)
TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.user_roles
    OWNER TO postgres;

COMMENT ON TABLE public.user_roles IS 'Ролі користувачів (admin, user, manager тощо).';
