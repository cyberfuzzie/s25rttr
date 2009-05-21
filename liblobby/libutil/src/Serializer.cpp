#include "Serializer.h"


Serializer::Serializer() : data(0), buffer_length(0), length(0), pos(0)
{
}

Serializer::Serializer(const void * const data, const unsigned initial_size)
: data(new unsigned char[initial_size]), buffer_length(initial_size), length(initial_size), pos(0)
{
	memcpy(this->data, data, initial_size);
}


Serializer::~Serializer()
{
	delete [] data;
}

/// Aufräumen
void Serializer::Clear()
{
	delete [] data;
	buffer_length = 0;
	length = 0;
	pos = 0;
}

void Serializer::Realloc(const unsigned int length)
{
	if(this->buffer_length == 0)
			this->buffer_length = 64;

	// speicher vergrößern
	while(this->buffer_length < length)
		this->buffer_length *= 2;

	if(data == 0)
	{
		// neu anlegen
		data = new unsigned char[this->buffer_length];
		memset(data, 0, sizeof(unsigned char)*this->buffer_length);
	}
	else
	{
		// umkopieren (vergrößern)
		unsigned char *ndata = new unsigned char[this->buffer_length];
		memcpy(ndata, data, length);

		delete[] data;
		data = ndata;
	}
}


/// Erweitert ggf. Speicher um add_length
void Serializer::ExtendMemory(const unsigned add_length)
{
	if(length + add_length < this->buffer_length)
		Realloc(length+add_length);

}

