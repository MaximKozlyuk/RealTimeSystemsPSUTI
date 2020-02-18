sudo scp -P 2022 <local_file_name> root@127.0.0.1:/root/<user_folder>

gcc $CFLAGS $LDFLAGS -lnative -lrtdk <c_source_name> -o <result_name>