create table iot_entry
(
    id int auto_increment,
    created_at datetime default CURRENT_TIMESTAMP null,
    sensor varchar(255) null,
    temperature varchar(255) null,
    humidity varchar(255) null,
    constraint iot_entry_id_uindex
        unique (id)
);

alter table iot_entry
    add primary key (id);

