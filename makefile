all:
	g++ main.cpp source/geo.cpp source/Geoapp.cpp source/Input.cpp source/Construction.cpp -o app -g -Wall -lsfml-window -lsfml-system -lsfml-graphics -fsanitize=leak -fsanitize=address
