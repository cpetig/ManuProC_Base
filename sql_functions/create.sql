
drop function kw2date(int4);
create function kw2date(int4) returns text as
	'/usr/lib/postgresql/lib/kw2date.so' language 'c';
update pg_proc set provolatile='i' where proname='kw2date';

drop function atoi(text);
create function atoi(text) returns int4 as
	'/usr/lib/postgresql/lib/atoi.so', 'myatoi' language 'c';
update pg_proc set provolatile='i' where proname='atoi';

drop function ean_check(text);
create function ean_check(text) returns int4 as
	'/usr/lib/postgresql/lib/ean_check.so','ean_check' language 'c';
update pg_proc set provolatile='i' where proname='ean_check';

drop function ean_check(int4);
create function ean_check(int4) returns int4 as
	'/usr/lib/postgresql/lib/ean_check_int.so','ean_check' language 'c';
update pg_proc set provolatile='i' where proname='ean_check';

