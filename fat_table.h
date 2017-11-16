class __attribute__((__packed__)) fat_table{
	public:
	    unsigned char tabela_FAT[bytes_per_sector];
        unsigned int fat_deslocamento;
        unsigned int fat_setor;
        unsigned int desloc_entrada;
        unsigned short valor_tabela;
};

fat_table::fat_table(unsigned char tabela_FAT[bytes_per_sector], unsigned int fat_deslocamento,
                     unsigned int fat_setor, unsigned int desloc_entrada,
                     unsigned short valor_tabela){

	this->tabela_FAT[bytes_per_sector] = tabela_FAT[bytes_per_sector];
	this->fat_deslocamento = cluster_atual * 2;
	this->fat_setor = fat_primeiro_setor + (fat_deslocamento / bytes_per_sector);
	this->desloc_entrada = fat_deslocamento % bytes_per_sector;;
	this->valor_tabela = *(unsigned short*)&tabela_FAT[desloc_entrada];;
}
