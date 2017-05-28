
create table logs (
    id serial,
    type int,
    sender int,
    senttime bigint,
    text varchar(256),
    temp float,
    alt int
);