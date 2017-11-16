class __attribute__((__packed__)) directory_entry{
	public:
		unsigned short name_file;
		unsigned short attribute_file;
		unsigned short creation_time;
		unsigned short creation_date;
		unsigned short last_acess_date;
		unsigned short last_modification_time;
		unsigned short last_modification_date;
		unsigned short first_cluster;
		unsigned short size_file;
};

directory_entry::directory_entry(unsigned short name_file, unsigned short attribute_file,
                                 unsigned short creation_time, unsigned short creation_date,
                                 unsigned short last_acess_date, unsigned short last_modification_time,
                                 unsigned short last_modification_date, unsigned short first_cluster,
                                 unsigned short size_file){

	this->name_file = name_file;
	this->attribute_file = attribute_file;
	this->creation_time = creation_time;
	this->creation_date = creation_date;
	this->last_acess_date = last_acess_date;
	this->last_modification_time = last_modification_time;
	this->last_modification_date = last_modification_date;
	this->first_cluster = first_cluster;
	this->size_file = size_file;
}
