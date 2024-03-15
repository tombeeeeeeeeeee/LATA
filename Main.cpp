

#include "Lophics.h"

int main()
{
	Lophics lophics;
	lophics.Start();
	while (!glfwWindowShouldClose(lophics.window)) {
		lophics.Update();
	}
	lophics.Stop();
}
