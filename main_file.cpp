/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include <objloader.hpp>
#include <iostream>


using namespace std;

GLuint tex;


int licznik = 0;
int countVertex1 = 0;
int numerobiektu = 0;
vector< glm::vec3 > vertices;
vector< glm::vec2 > uvs;
vector< glm::vec3 > normals;
vector< glm::vec4 > colors;



vector< glm::vec4  > wektor_id;



vector<float> wektor_wierzch_id;
vector<float> wektor_uvs_id;
vector<float> wektor_normals_id;
//vector<float> v_count1;
vector<int> liczby_wierzch;
vector<float> kolory;


vector<vector<float>> tworzona_tablica_vx;
vector<vector<float>> tworzona_tablica_uvs;
vector<vector<float>> tworzona_tablica_normals;
vector<vector<float>> tworzona_tablica_kolor;


float speed = PI; //Prędkość kątowa obrotu obiektu
void rysujobiekt(int numerobiektu)
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);


	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, &tworzona_tablica_vx[numerobiektu][0]);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, &tworzona_tablica_normals[numerobiektu][0]);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, &tworzona_tablica_uvs[numerobiektu][0]);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, &tworzona_tablica_kolor[numerobiektu][0]);

	glDrawArrays(GL_TRIANGLES, 0, liczby_wierzch[numerobiektu]);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	glClearColor(0.1, 0.1, 0.1, 0);


}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);
	//cout << "ERROR:" << error << "\n";
	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	//glBindTexture(GL_TEXTURE_2D, tex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}
//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	glClearColor(0, 0, 0, 1);//Ustaw czarny kolor czyszczenia ekranu
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości na pikselach
	//glfwSetKeyCallback(window);


}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	glDeleteTextures(1, &tex);
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

}


void texTest(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
	spLambertTextured->use();

	glUniformMatrix4fv(spLambertTextured->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambertTextured->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(M));


}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyszczenie bufora kolorów i bufora głębokości

	glEnable(GL_DEPTH_TEST);
	//	for (int y=0; y < tablicawartosci.size(); y++;)

	glm::mat4 P = glm::perspective(glm::radians(10.0f), 1.0f, 1.0f, 50.0f); //Wyliczenie macierzy rzutowania
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, -40.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Wyliczenie macierzy widoku


	//glDisable(GL_CULL_FACE);
//spConstant->use();//Aktywacja programu cieniującego
//glUniformMatrix4fv(spConstant->u("P"), 1, false, glm::value_ptr(P)); //Załadowanie macierzy rzutowania do programu cieniującego
//glUniformMatrix4fv(spConstant->u("V"), 1, false, glm::value_ptr(V)); //Załadowanie macierzy widoku do programu cieniującego
	
	for (int x = 0; x < numerobiektu; x++)
	{


		//	glm::mat4 M = glm::translate(M, glm::vec3(translater1[g], translater2[g], translater3[g]));
			//translater1++;
			//translater2++;
			//translater3++;


		glm::mat4 M = glm::mat4(1.0f);

		M = glm::rotate(M, angle, glm::vec3(x, 1.0f, 0.0f));

		//glUniformMatrix4fv(spConstant->u("M"), 1, false, glm::value_ptr(M));  //Załadowanie macierzy modelu do programu cieniującego
		

		rysujobiekt(x); //Narysowanie obiektu
		texTest(P, V, M);

	}
	//---Poniższy kawałek kodu powtarzamy dla każdego obiektu
	//Obliczanie macierzy modelu



	//Skopiowanie bufora ukrytego do widocznego. Z reguły ostatnie polecenie w procedurze drawScene.
	glfwSwapBuffers(window);
}


void wektorowanie()
{
	//vx
	for (int i = 0; i < vertices.size(); i++)
	{

		wektor_wierzch_id.push_back(vertices[i].x);
		wektor_wierzch_id.push_back(vertices[i].y);
		wektor_wierzch_id.push_back(vertices[i].z);
		wektor_wierzch_id.push_back(1.0);

	}
	tworzona_tablica_vx.push_back(wektor_wierzch_id);

	//uvs

	for (int i = 0; i < uvs.size(); i++)
	{

		wektor_uvs_id.push_back(vertices[i].x);
		wektor_uvs_id.push_back(vertices[i].y);
	}
	tworzona_tablica_uvs.push_back(wektor_uvs_id);

	//normals


	for (int i = 0; i < normals.size(); i++)
	{

		wektor_normals_id.push_back(vertices[i].x);
		wektor_normals_id.push_back(vertices[i].y);
		wektor_normals_id.push_back(vertices[i].z);
		wektor_normals_id.push_back(0.0);
	}
	tworzona_tablica_normals.push_back(wektor_normals_id);

	countVertex1 = vertices.size();
	cout << countVertex1;


	for (int b = 0; b < wektor_wierzch_id.size(); b += 4)
	{
		kolory.push_back(1.0);

		kolory.push_back(0.0);
		kolory.push_back(0.0);
		kolory.push_back(1.0);

	}

	tworzona_tablica_kolor.push_back(kolory);


	wektor_wierzch_id.clear();
	wektor_uvs_id.clear();
	wektor_normals_id.clear();
	liczby_wierzch.push_back(countVertex1);
	countVertex1 = 0;

}




int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1000, 1000, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące


	//
	//translater1.push_back(20.0f);
	//translater2.push_back(10.0f);
	//translater3.push_back(20.0f);

	int zmienna = 0;
	float angle = 0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Wyzeruj timer
	//wywfunkcji
		//wagonik1



	tex = readTexture("niebo.png");

	loadOBJ("car4.obj", vertices, uvs, normals); // vertices - co 3 liczby 1.0f, uvs nic, normals - co 3 liczby 0.0f	
	wektorowanie();
	numerobiektu++;


	//	loadOBJ("CarWheel.obj", vertices, uvs, normals); // vertices - co 3 liczby 1.0f, uvs nic, normals - co 3 liczby 0.0f	
	//	wektorowanie();
	//	numerobiektu++;

		//loadOBJ("cubbeee.obj", vertices, uvs, normals); // vertices - co 3 liczby 1.0f, uvs nic, normals - co 3 liczby 0.0f	
		//wektorowanie();
		//numerobiektu++;

		//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{

		angle += speed * glfwGetTime(); //Oblicz przyrost kąta po obrocie
		glfwSetTime(0); //Wyzeruj timer
		drawScene(window, angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);
	glDeleteTextures(1, &tex);
	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}