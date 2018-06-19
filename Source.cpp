#include<GL/glut.h>
#include<stdio.h>
#include<math.h>

#define linearLimit 100
#define squareLimit 20
#define PI 3.14159

float winSize = 5.0;
float winSizeT = 5.0, winSizeB = 5.0, winSizeL = 5.0, winSizeR = 5.0;
int winWidth, winHeight;
float angle = 0.0, axis[3];
bool trackingMouse = false;
bool redrawContinue = false;
bool trackballMove = false;
float lastPos[3] = { 0, 0, 0 };
int startX, startY;
char equation[99];

float xco = 0, yco = 0, zco = 0, constant = 0;
int dimen = 0, xpower = 0, ypower = 0, zpower = 0;
bool xa = false, ya = false, za = false;
bool solutionExist, invalidEquation;

void shape(int w, int h)
{
	GLfloat ratio = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, w, h);
	winWidth = w;
	winHeight = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w >= h)
		glOrtho(-winSizeL * ratio, winSizeR * ratio, -winSizeB, winSizeT, -8 * winSize, 8 * winSize);
	else
		glOrtho(-winSizeL, winSizeR, -winSizeB / ratio, winSizeT / ratio, -8 * winSize, 8 * winSize);
	glMatrixMode(GL_MODELVIEW);
}

void singleLinear(float x, float y, float z)
{
	glColor3f(1, 0, 1);
	glPointSize(10.0);
	glBegin(GL_POINTS);
	glVertex3f(x, y, z);
	glEnd();
	glPointSize(1.0);
}

void doubleLinear()
{
	glLineWidth(2.0);
	glBegin(GL_LINES);
	if (xa && ya)
	{
		glVertex3f(-linearLimit, (constant - (xco * (-linearLimit))) / yco, 0);
		glVertex3f(linearLimit, (constant - (xco * linearLimit)) / yco, 0);
	}
	else if (ya && za)
	{
		glVertex3f(0, -linearLimit, (constant - (yco * (-linearLimit))) / zco);
		glVertex3f(0, linearLimit, (constant - (yco * linearLimit)) / zco);
	}
	else if (xa && za)
	{
		glVertex3f((constant - (zco * (-linearLimit))) / xco, 0, -linearLimit);
		glVertex3f((constant - (zco * linearLimit)) / xco, 0, linearLimit);
	}

	glEnd();
	glLineWidth(1.0);
}

void tripleLinear()
{
	glColor3f(1, 0, 0);
	float k = fabs(constant);
	float inif;
	if (k <= 4)
		inif = 4;
	else if (k <= 10)
		inif = k;
	else
		inif = 10;
	for (float z = -inif; z <= inif; z += 0.1)
	{
		glBegin(GL_LINE_STRIP);
		for (float y = -inif; y <= inif; y += 0.1)
		{
			glVertex3f((constant - yco * y - zco * z) / xco, y, z);
		}
		glEnd();
	}
	glColor3f(1, 0, 1);
	for (float y = -inif; y <= inif; y += 0.1)
	{
		glBegin(GL_LINE_STRIP);
		for (float z = -inif; z <= inif; z += 0.1)
		{
			glVertex3f((constant - yco * y - zco * z) / xco, y, z);
		}
		glEnd();
	}
}

void singleSquare(float rhs)
{
	glColor3f(1, 0, 1);
	if (rhs >= 0)
	{
		glPointSize(10.0);
		glBegin(GL_POINTS);
		if (xa)
		{
			glVertex3f(sqrt(constant / xco), 0, 0);
			glVertex3f(-sqrt(constant / xco), 0, 0);
		}
		else if (ya)
		{
			glVertex3f(0, sqrt(constant / yco), 0);
			glVertex3f(0, -sqrt(constant / yco), 0);
		}
		else if (za)
		{
			glVertex3f(0, 0, sqrt(constant / zco));
			glVertex3f(0, 0, -sqrt(constant / zco));
		}
		glEnd();
		glPointSize(1.0);
	}
}

void doubleSquared(bool xy, bool yz, bool zx)
{
	glLineWidth(2.0);
	glColor3f(0, 0, 0);
	float step;
	float startx, starty, startz;
	if (xy)
	{
		if (xco > 0 && yco > 0 && constant > 0 || xco < 0 && yco < 0 && constant < 0)
		{
			startx = sqrt(fabs(constant/xco));
			glBegin(GL_LINE_LOOP);
			step = xco / 100;

			for (float x = -startx; x <= startx; x += step)
				if ((constant - xco * x * x) / yco >= 0)
					glVertex3f(x, sqrt((constant - xco * x * x) / yco), 0);

			for (float x = startx; x >= -startx; x -= step)
				if ((constant - xco * x * x) / yco >= 0)
					glVertex3f(x, -sqrt((constant - xco * x * x) / yco), 0);

			glEnd();
		}
		else if (xco < 0 && yco > 0 && constant >= 0 || xco > 0 && yco < 0 && constant <= 0)
		{
			startx = squareLimit;

			glBegin(GL_LINE_STRIP);
			for (float x = -startx; x <= startx; x += 0.01)
				if ((constant - xco * x * x) / yco >= 0)
					glVertex3f(x, sqrt((constant - xco * x * x) / yco), 0);
			glEnd();

			glBegin(GL_LINE_STRIP);
			for (float x = -startx; x <= startx; x += 0.01)
				if ((constant - xco * x * x) / yco >= 0)
					glVertex3f(x, -sqrt((constant - xco * x * x) / yco), 0);
			glEnd();
		}
		else
		{
			starty = squareLimit;

			glBegin(GL_LINE_STRIP);
			for (float y = -starty; y <= starty; y += 0.01)
				if ((constant - yco * y * y) / xco >= 0)
					glVertex3f(sqrt((constant - yco * y *y) / xco), y, 0);
			glEnd();

			glBegin(GL_LINE_STRIP);
			for (float y = -starty; y <= starty; y += 0.01)
				if ((constant - yco * y * y) / xco >= 0)
					glVertex3f(-sqrt((constant - yco * y *y) / xco), y, 0);
			glEnd();
		}
	}
	else if (yz)
	{
		if (yco > 0 && zco > 0 && constant > 0 || yco < 0 && zco < 0 && constant < 0)
		{
			starty = sqrt(fabs(constant/yco));
			glBegin(GL_LINE_LOOP);

			for (float y = -starty; y <= starty; y += 0.01)
				if ((constant - yco * y * y) / zco >= 0)
					glVertex3f(0, y, sqrt((constant - yco * y * y) / zco));

			for (float y = starty; y >= -starty; y -= 0.01)
				if ((constant - yco * y * y) / zco >= 0)
					glVertex3f(0, y, -sqrt((constant - yco * y * y) / zco));

			glEnd();
		}
		else if (yco < 0 && zco > 0 && constant >= 0 || yco > 0 && zco < 0 && constant <= 0)
		{
			starty = squareLimit;

			glBegin(GL_LINE_STRIP);
			for (float y = -starty; y <= starty; y += 0.01)
				if ((constant - yco * y * y) / zco >= 0)
					glVertex3f(0, y, sqrt((constant - yco * y * y) / zco));
			glEnd();

			glBegin(GL_LINE_STRIP);
			for (float y = -starty; y <= starty; y += 0.01)
				if ((constant - yco * y * y) / zco >= 0)
					glVertex3f(0, y, -sqrt((constant - yco * y * y) / zco));
			glEnd();
		}
		else
		{
			startz = squareLimit;

			glBegin(GL_LINE_STRIP);
			for (float z = -startz; z <= startz; z += 0.01)
				if ((constant - zco * z * z) / yco >= 0)
					glVertex3f(0, sqrt((constant - zco * z *z) / yco), z);
			glEnd();

			glBegin(GL_LINE_STRIP);
			for (float z = -startz; z <= startz; z += 0.01)
				if ((constant - zco * z * z) / yco >= 0)
					glVertex3f(0, -sqrt((constant - zco * z *z) / yco), z);
			glEnd();
		}
	}
	else if (zx)
	{
		if (zco > 0 && xco > 0 && constant > 0 || zco < 0 && xco < 0 && constant < 0)
		{
			startz = sqrt(fabs(constant/zco));

			glBegin(GL_LINE_LOOP);

			for (float z = -startz; z <= startz; z += 0.01)
				if ((constant - zco * z * z) / xco >= 0)
					glVertex3f(sqrt((constant - zco * z * z) / xco), 0, z);

			for (float z = startz; z >= -startz; z -= 0.01)
				if ((constant - zco * z * z) / xco >= 0)
					glVertex3f(-sqrt((constant - zco * z * z) / xco), 0, z);

			glEnd();
		}
		else if (zco < 0 && xco > 0 && constant >= 0 || zco > 0 && xco < 0 && constant <= 0)
		{
			startz = squareLimit;

			glBegin(GL_LINE_STRIP);
			for (float z = -startz; z <= startz; z += 0.01)
				if ((constant - zco * z * z) / xco >= 0)
					glVertex3f(sqrt((constant - zco * z * z) / xco), 0, z);
			glEnd();

			glBegin(GL_LINE_STRIP);
			for (float z = -startz; z <= startz; z += 0.01)
				if ((constant - zco * z * z) / xco >= 0)
					glVertex3f(-sqrt((constant - zco * z * z) / xco), 0, z);
			glEnd();
		}
		else
		{
			startx = squareLimit;

			glBegin(GL_LINE_STRIP);
			for (float x = -startx; x <= startx; x += 0.01)
				if ((constant - xco * x * x) / zco >= 0)
					glVertex3f(x, 0, sqrt((constant - xco * x *x) / zco));
			glEnd();

			glBegin(GL_LINE_STRIP);
			for (float x = -startx; x <= startx; x += 0.01)
				if ((constant - xco * x * x) / zco >= 0)
					glVertex3f(x, 0, -sqrt((constant - xco * x *x) / zco));
			glEnd();
		}
	}
	glLineWidth(1.0);
}

void doubleOneSquared(bool xy, bool yz, bool zx)
{
	glLineWidth(2.0);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);

	if (xy)
	{
		if (xpower == 2)
		{
			for (float x = -linearLimit; x <= linearLimit; x += 0.01)
				glVertex3f(x, (constant - xco * x * x) / yco, 0);
		}
		else
		{
			for (float y = -linearLimit; y <= linearLimit; y += 0.01)
				glVertex3f((constant - yco * y * y) / xco, y, 0);
		}
	}
	else if (yz)
	{
		if (ypower == 2)
		{
			for (float y = -linearLimit; y <= linearLimit; y += 0.01)
				glVertex3f(0, y, (constant - yco * y * y) / zco);
		}
		else
		{
			for (float z = -linearLimit; z <= linearLimit; z += 0.01)
				glVertex3f(0, (constant - zco * z * z) / yco, z);
		}
	}
	else if (zx)
	{
		if (zpower == 2)
		{
			for (float z = -linearLimit; z <= linearLimit; z += 0.01)
				glVertex3f((constant - zco * z * z) / xco, 0, z);
		}
		else
		{
			for (float x = -linearLimit; x <= linearLimit; x += 0.01)
				glVertex3f(x, 0, (constant - xco * x * x) / zco);
		}
	}

	glEnd();
	glLineWidth(1.0);

}

void tripleTwoLinearOneSquared(bool xy, bool yz, bool zx)
{
	float k = fabs(constant);
	float inif;
	if (k <= 4)
		inif = 4;
	else if (k <= 10)
		inif = k;
	else
		inif = 10;
	if (xy)
	{
		for (float x = -inif; x <= inif; x += 0.1)
		{
			glBegin(GL_LINE_STRIP);
			glColor3f(1, 0, 0);
			for (float z = -inif; z <= inif; z += 0.1)
			{
				if (z > 0)
					glColor3f(0, 1, 0);
				glVertex3f(x, (constant - xco * x - zco * z * z) / yco, z);
			}
			glEnd();
		}
		for (float z = -inif; z <= inif; z += 0.1)
		{
			glBegin(GL_LINE_STRIP);
			glColor3f(1, 0, 0);
			if (z > 0)
				glColor3f(0, 1, 0);
			glVertex3f(-inif, (constant + xco * inif - zco * z * z) / yco, z);
			glVertex3f(inif, (constant - xco * inif - zco * z * z) / yco, z);
			glEnd();
		}
	}
	else if (yz)
	{
		for (float y = -inif; y <= inif; y += 0.1)
		{
			glBegin(GL_LINE_STRIP);
			glColor3f(1, 0, 0);
			for (float x = -inif; x <= inif; x += 0.1)
			{
				if (x > 0)
					glColor3f(0, 1, 0);
				glVertex3f(x, y, (constant - yco * y - xco * x * x) / zco);
			}
			glEnd();
		}
		for (float x = -inif; x <= inif; x += 0.1)
		{
			glBegin(GL_LINE_STRIP);
			glColor3f(1, 0, 0);
			if (x > 0)
				glColor3f(0, 1, 0);
			glVertex3f(x, -inif, (constant + yco * inif - xco * x * x) / zco);
			glVertex3f(x, inif, (constant - yco * inif - xco * x * x) / zco);
			glEnd();
		}
	}
	else if (zx)
	{
		for (float z = -inif; z <= inif; z += 0.1)
		{
			glBegin(GL_LINE_STRIP);
			glColor3f(1, 0, 0);
			for (float y = -inif; y <= inif; y += 0.1)
			{
				if (y > 0)
					glColor3f(0, 1, 0);
				glVertex3f((constant - zco * z - yco * y * y) / yco, y, z);
			}
			glEnd();
		}
		for (float y = -inif; y <= inif; y += 0.1)
		{
			glBegin(GL_LINE_STRIP);
			glColor3f(1, 0, 0);
			if (y > 0)
				glColor3f(0, 1, 0);
			glVertex3f((constant + zco * inif - yco * y * y) / yco, y, -inif);
			glVertex3f((constant - zco * inif - yco * y * y) / yco, y, inif);
			glEnd();
		}
	}
}

void tripleOneLinearTwoSquared(bool xy, bool yz, bool zx)
{
	float k = fabs(constant);
	float inif;
	if (k <= 4)
		inif = 4;
	else if (k <= 10)
		inif = k;
	else
		inif = 10;
	if (xy)
	{
		for (float x = -inif; x <= inif; x += 0.1)
		{
			glColor3f(1.0, 0, 1);
			glBegin(GL_LINE_STRIP);
			for (float y = -inif; y <= inif; y += 0.1)
			{
				glVertex3f(x, y, (constant - xco *x * x - yco * y * y) / zco);
				if (y > 0)
					glColor3f(1, 0, 0);
			}
			glEnd();
		}
		for (float y = -inif; y <= inif; y += 0.1)
		{
			glColor3f(1.0, 0, 1);
			glBegin(GL_LINE_STRIP);
			for (float x = -inif; x <= inif; x += 0.1)
			{
				glVertex3f(x, y, (constant - xco *x * x - yco * y * y) / zco);
				if (x > 0)
					glColor3f(1, 0, 0);
			}
			glEnd();
		}
	}
	else if (yz)
	{
		for (float y = -inif; y <= inif; y += 0.1)
		{
			glColor3f(1, 0, 1);
			glBegin(GL_LINE_STRIP);
			for (float z = -inif; z <= inif; z += 0.1)
			{
				glVertex3f((constant - yco * y * y - zco * z * z) / xco, y, z);
				if (z > 0)
					glColor3f(1, 0, 0);
			}
			glEnd();
		}
		for (float z = -inif; z <= inif; z += 0.1)
		{
			glColor3f(1, 0, 1);
			glBegin(GL_LINE_STRIP);
			for (float y = -inif; y <= inif; y += 0.1)
			{
				glVertex3f((constant - yco * y * y - zco * z * z) / xco, y, z);
				if (z > 0)
					glColor3f(1, 0, 0);
			}
			glEnd();
		}
	}
	else if (zx)
	{
		for (float z = -inif; z <= inif; z += 0.1)
		{
			glColor3f(1, 0, 1);
			glBegin(GL_LINE_STRIP);
			for (float x = -inif; x <= inif; x += 0.1)
			{
				glVertex3f(x, (constant - xco * x * x - zco * z * z) / yco, z);
				if (x > 0)
					glColor3f(1, 0, 0);
			}
			glEnd();
		}
		for (float x = -inif; x <= inif; x += 0.1)
		{
			glColor3f(1, 0, 1);
			glBegin(GL_LINE_STRIP);
			for (float z = -inif; z <= inif; z += 0.1)
			{
				glVertex3f(x, (constant - xco * x * x - zco * z * z) / yco, z);
				if (x > 0)
					glColor3f(1, 0, 0);
			}
			glEnd();
		}
	}
}

void tripleSquared()
{
	float pStep;
	float sStep;
	float inif;
	float k = fabs(constant);
	if (k <= 4)
		inif = 4;
	else if (k <= 10)
		inif = k;
	else
		inif = 10;
	if (constant != 0)
	{
		pStep = (float)inif / 12;
		if ((fabs(xco) + fabs(yco) + fabs(zco)) < k)
			sStep = (float)(fabs(xco) + fabs(yco) + fabs(zco)) / (k * 30);
		else
			sStep = (float)k / ((fabs(xco) + fabs(yco) + fabs(zco)) * 30);
	}
	else
	{
		inif = 4;
		pStep = 0.5;
		sStep = 0.05;
	}
	sStep = sStep>0.01 ? 0.01 : sStep;
	pStep = pStep>0.2 ? 0.2 : pStep;
	glColor3f(1, 0, 0);
	float range;

	glLineWidth(2.0);
	glPushMatrix();
	glScalef(1 / sqrt(fabs(xco)), 1 / sqrt(fabs(yco)), 1 / sqrt(fabs(zco)));
	//Sphere
	if (xco > 0 && yco > 0 && zco > 0 && constant > 0 || xco < 0 && yco < 0 && zco < 0 && constant < 0)
	{
		inif = sqrt(k);
		for (float z = -inif; z <= inif + pStep; z += pStep)
		{
			if (k < 1)
				range = 1;
			else if (fabs(z * 2) < k)
				range = k;
			else
				range = fabs(2 * z);
			if (z >= 0)
				glColor3f(0, 0, 1);
			glBegin(GL_LINE_LOOP);
			for (float x = -range; x <= range; x += sStep)
				if ((k - x * x - z * z) >= 0)
					glVertex3f(x, sqrt(k - x * x - z * z), z);
			for (float x = range; x >= -range; x -= sStep)
				if ((k - x * x - z * z) >= 0)
					glVertex3f(x, -sqrt(k - x * x - z * z), z);
			glEnd();
		}
		glColor3f(0, 0, 0);
		for (int i = 0; i < 12; i++)
		{
			glRotatef(180.0 / 12, 0, 0, 1);
			glBegin(GL_LINE_LOOP);
			for (float z = -inif; z <= inif; z += sStep)
				if ((k - z * z) >= 0)
					glVertex3f(0, sqrt(k - z * z), z);
			for (float z = inif; z >= -inif; z -= sStep)
				if ((k - z * z) >= 0)
					glVertex3f(0, -sqrt(k - z * z), z);
			glEnd();
		}
	}
	//Worm Hole along z-axis
	else if (xco > 0 && yco > 0 && zco < 0 && constant >= 0 || xco < 0 && yco < 0 && zco > 0 && constant <= 0)
	{
		for (float z = -inif; z <= inif; z += pStep)
		{
			if (fabs(z) < 1 && k < 1)
				range = 1.5;
			else if (fabs(z * 2) < k)
				range = k;
			else
				range = fabs(2 * z);
			glColor3f(1, 0, 0);
			glBegin(GL_LINE_LOOP);
			for (float x = -range; x <= range; x += sStep)
				if ((k - x * x + z * z) >= 0)
					glVertex3f(x, sqrt(k - x * x + z * z), z);
			glColor3f(1, 0, 1);
			for (float x = range; x >= -range; x -= sStep)
				if ((k - x * x + z * z) >= 0)
					glVertex3f(x, -sqrt(k - x * x + z * z), z);
			glEnd();
		}
		glColor3f(0, 0, 0);
		for (int i = 0; i < 24; i++)
		{
			glRotatef(15, 0, 0, 1);
			glBegin(GL_LINE_STRIP);
			for (float z = -inif; z <= inif; z += sStep)
				if ((k + z * z) >= 0)
					glVertex3f(0, sqrt(k + z * z), z);
			glEnd();
		}
	}
	//Worm Hole along x-axis
	else if (xco < 0 && yco > 0 && zco > 0 && constant >= 0 || xco > 0 && yco < 0 && zco < 0 && constant <= 0)
	{
		for (float x = -inif; x <= inif; x += pStep)
		{
			if (fabs(x) < 1 && k < 1)
				range = 1.5;
			else if (fabs(x * 2) < k)
				range = k;
			else
				range = fabs(2 * x);
			glColor3f(1, 0, 0);
			glBegin(GL_LINE_LOOP);
			for (float z = -range; z <= range; z += sStep)
				if ((k + x * x - z * z) >= 0)
					glVertex3f(x, sqrt(k + x * x - z * z), z);
			glColor3f(1, 0, 1);
			for (float z = range; z >= -range; z -= sStep)
				if ((k + x * x - z * z) >= 0)
					glVertex3f(x, -sqrt(k + x * x - z * z), z);
			glEnd();
		}
		glColor3f(0, 0, 0);
		for (int i = 0; i < 24; i++)
		{
			glRotatef(15, 1, 0, 0);
			glBegin(GL_LINE_STRIP);
			for (float x = -inif; x <= inif; x += sStep)
				if ((k + x * x) >= 0)
					glVertex3f(x, sqrt(k + x * x), 0);
			glEnd();
		}
	}
	//Worm Hole along y-axis
	else if (xco > 0 && yco < 0 && zco > 0 && constant >= 0 || xco < 0 && yco > 0 && zco < 0 && constant <= 0)
	{
		for (float y = -inif; y <= inif; y += pStep)
		{
			if (fabs(y) < 1 && k < 1)
				range = 1.5;
			else if (fabs(y * 2) < k)
				range = k;
			else
				range = fabs(2 * y);
			glColor3f(1, 0, 0);
			glBegin(GL_LINE_LOOP);
			for (float x = -range; x <= range; x += sStep)
				if ((k - x * x + y * y) >= 0)
					glVertex3f(x, y, sqrt(k - x * x + y * y));
			glColor3f(1, 0, 1);
			for (float x = range; x >= -range; x -= sStep)
				if ((k - x * x + y * y) >= 0)
					glVertex3f(x, y, -sqrt(k - x * x + y * y));
			glEnd();
		}
		glColor3f(0, 0, 0);
		for (int i = 0; i < 24; i++)
		{
			glRotatef(15, 0, 1, 0);
			glBegin(GL_LINE_STRIP);
			for (float y = -inif; y <= inif; y += sStep)
				if ((k + y * y) >= 0)
					glVertex3f(sqrt(k + y * y), y, 0);
			glEnd();
		}
	}
	//Concave-Convex Lens along z-axis
	else if (xco < 0 && yco < 0 && zco > 0 && constant >= 0 || xco > 0 && yco > 0 && zco < 0 && constant <= 0)
	{
		for (float z = -1.5*inif; z <= 1.5*inif + pStep; z += pStep)
		{
			if (fabs(z * 2) < k)
				range = k;
			else
				range = fabs(2 * z);
			if (z >= 0)
				glColor3f(1, 0, 1);
			glBegin(GL_LINE_LOOP);
			for (float x = -range; x <= range; x += sStep)
				if ((-k - x*x + z*z) >= 0)
					glVertex3f(x, sqrt(-k - x*x + z*z), z);
			for (float x = range; x >= -range; x -= sStep)
				if ((-k - x*x + z*z) >= 0)
					glVertex3f(x, -sqrt(-k - x*x + z*z), z);
			glEnd();
		}
		glColor3f(0, 0, 0);
		for (int i = 0; i < 12; i++)
		{
			glRotatef(15, 0, 0, 1);
			glBegin(GL_LINE_STRIP);
			for (float x = 1.5*inif; x >= -1.5*inif; x -= sStep)
				if (k + x*x >= 0)
					glVertex3f(x, 0, sqrt(k + x*x));
			glEnd();
			glBegin(GL_LINE_STRIP);
			for (float y = 1.5*inif; y >= -1.5*inif; y -= sStep)
				if ((k + y*y) >= 0)
					glVertex3f(0, y, -sqrt(k + y*y));
			glEnd();
		}
	}
	//Concave-Convex Lens along x-axis
	else if (xco > 0 && yco < 0 && zco < 0 && constant >= 0 || xco < 0 && yco > 0 && zco > 0 && constant <= 0)
	{
		for (float x = -1.5*inif; x <= 1.5*inif + pStep; x += pStep)
		{
			if (fabs(x * 2) < k)
				range = k;
			else
				range = fabs(2 * x);
			if (x >= 0)
				glColor3f(1, 0, 1);
			glBegin(GL_LINE_LOOP);
			for (float z = -range; z <= range; z += sStep)
				if ((-k + x*x - z*z) >= 0)
					glVertex3f(x, sqrt(-k + x*x - z*z), z);
			for (float z = range; z >= -range; z -= sStep)
				if ((-k + x*x - z*z) >= 0)
					glVertex3f(x, -sqrt(-k + x*x - z*z), z);
			glEnd();
		}
		glColor3f(0, 0, 0);
		for (int i = 0; i < 12; i++)
		{
			glRotatef(15, 1, 0, 0);
			glBegin(GL_LINE_STRIP);
			for (float z = 1.5*inif; z >= -1.5*inif; z -= sStep)
				if ((k + z*z) >= 0)
					glVertex3f(sqrt(k + z*z), 0, z);
			glEnd();
			glBegin(GL_LINE_STRIP);
			for (float y = 1.5*inif; y >= -1.5*inif; y -= sStep)
				if ((k + y*y) >= 0)
					glVertex3f(-sqrt(k + y*y), y, 0);
			glEnd();
		}
	}
	//Concave-Convex Lens along y-axis
	else if (xco < 0 && yco > 0 && zco < 0 && constant >= 0 || xco > 0 && yco < 0 && zco > 0 && constant <= 0)
	{
		for (float y = -1.5*inif; y <= 1.5*inif; y += pStep)
		{
			if (fabs(y * 2) < k)
				range = k;
			else
				range = fabs(2 * y);
			if (y >= 0)
				glColor3f(1, 0, 1);
			glBegin(GL_LINE_LOOP);
			for (float z = -range; z <= range; z += sStep)
				if ((-k + y*y - z*z) >= 0)
					glVertex3f(sqrt(-k + y*y - z*z), y, z);
			for (float z = range; z >= -range; z -= sStep)
				if ((-k + y*y - z*z) >= 0)
					glVertex3f(-sqrt(-k + y*y - z*z), y, z);
			glEnd();
		}
		glColor3f(0, 0, 0);
		for (int i = 0; i < 12; i++)
		{
			glRotatef(15, 0, 1, 0);
			glBegin(GL_LINE_STRIP);
			for (float z = 1.5*inif; z >= -1.5*inif; z -= sStep)
				if ((k + z*z) >= 0)
					glVertex3f(0, sqrt(k + z*z), z);
			glEnd();
			glBegin(GL_LINE_STRIP);
			for (float x = 1.5*inif; x >= -1.5*inif; x -= sStep)
				if ((k + x*x) >= 0)
					glVertex3f(x, -sqrt(k + x*x), 0);
			glEnd();
		}
	}
	glPopMatrix();
	glLineWidth(1.0);
}

void drawGrid()
{
	float size;
	if (winSize < 4)
	{
		if ((int)(winSize * 10) % 10 != 0)
			size = 10 * (winSize + 0.5);
		else
			size = 10 * winSize;
	}
	else
	{
		if ((int)(winSize * 10) % 10 != 0)
			size = 4 * (winSize + 0.5);
		else
			size = 4 * winSize;
	}
	glColor4f(0.8, 0.8, 0.8, 1.0);
	if (winSize < 10)
	{
		glBegin(GL_LINES);
		if (dimen == 1)
		{
			if (xa || ya)
			{
				for (float i = -size; i <= size; i += 0.1)
				{
					glVertex3f(i, -size, 0);
					glVertex3f(i, size, 0);

					glVertex3f(-size, i, 0);
					glVertex3f(size, i, 0);
				}
			}
			else
			{
				for (float i = -size; i <= size; i += 0.1)
				{

					glVertex3f(-size, 0, i);
					glVertex3f(size, 0, i);

					glVertex3f(i, 0, -size);
					glVertex3f(i, 0, size);
				}
			}
		}
		else if (dimen == 2 || dimen == 3)
		{
			if (xa && ya)
			{
				for (float i = -size; i <= size; i += 0.1)
				{
					glVertex3f(i, -size, 0);
					glVertex3f(i, size, 0);

					glVertex3f(-size, i, 0);
					glVertex3f(size, i, 0);
				}
			}

			else if (xa && za)
			{
				for (float i = -size; i <= size; i += 0.1)
				{
					glVertex3f(-size, 0, i);
					glVertex3f(size, 0, i);

					glVertex3f(i, 0, -size);
					glVertex3f(i, 0, size);
				}
			}

			else
			{
				for (float i = -size; i <= size; i += 0.1)
				{
					glVertex3f(0, i, -size);
					glVertex3f(0, i, size);

					glVertex3f(0, -size, i);
					glVertex3f(0, size, i);
				}
			}
		}
		glEnd();
		glLineWidth(2.0);
	}
	glBegin(GL_LINES);
	if (dimen == 1)
	{
		if (xa || ya)
		{
			for (int i = -size; i <= size; i++)
			{
				glColor3f(0.0, 1.0, 0.0);
				glVertex3i(i, -size, 0);
				glVertex3i(i, size, 0);

				glColor3f(1.0, 0.0, 0.0);
				glVertex3i(-size, i, 0);
				glVertex3i(size, i, 0);
			}
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 2 * size);
			glVertex3f(0.0, 0.0, -2 * size);
		}
		else
		{
			for (int i = -size; i <= size; i++)
			{
				glColor3f(1.0, 0.0, 0.0);
				glVertex3i(-size, 0, i);
				glVertex3i(size, 0, i);

				glColor3f(0.0, 0.0, 1.0);
				glVertex3i(i, 0, -size);
				glVertex3i(i, 0, size);
			}
			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0.0, 2 * size, 0);
			glVertex3f(0.0, -2 * size, 0);
		}
	}
	else if (dimen == 2 || dimen == 3)
	{
		if (xa && ya)
		{
			for (int i = -size; i <= size; i++)
			{
				glColor3f(0.0, 1.0, 0.0);
				glVertex3i(i, -size, 0);
				glVertex3i(i, size, 0);

				glColor3f(1.0, 0.0, 0.0);
				glVertex3i(-size, i, 0);
				glVertex3i(size, i, 0);
			}
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 2 * size);
			glVertex3f(0.0, 0.0, -2 * size);
		}

		else if (xa && za)
		{
			for (int i = -size; i <= size; i++)
			{
				glColor3f(1.0, 0.0, 0.0);
				glVertex3i(-size, 0, i);
				glVertex3i(size, 0, i);

				glColor3f(0.0, 0.0, 1.0);
				glVertex3i(i, 0, -size);
				glVertex3i(i, 0, size);
			}
			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0.0, 2 * size, 0.0);
			glVertex3f(0.0, -2 * size, 0.0);
		}

		else
		{
			for (int i = -size; i <= size; i++)
			{
				glColor3f(0.0, 0.0, 1.0);
				glVertex3i(0, i, -size);
				glVertex3i(0, i, size);

				glColor3f(0.0, 1.0, 0.0);
				glVertex3i(0, -size, i);
				glVertex3i(0, size, i);
			}
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(2 * size, 0.0, 0.0);
			glVertex3f(-2 * size, 0.0, 0.0);
		}
	}
	glEnd();
	glColor3f(0, 0, 0);

	glRasterPos3d(0, 0, 0);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 48);
	if (winSize <= 14)
	{
		for (int i = 1; i <= 9; i++)
		{
			glRasterPos3d(i, 0, 0);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (i + 48));
			glRasterPos3d(0, i, 0);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (i + 48));
			glRasterPos3d(0, 0, i);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (i + 48));
		}
	}

	glLineWidth(1.0);

}

void trackball(int x, int y, int width, int height, float v[3])
{
	float d, a;
	v[0] = (2.0*x - width) / width;
	v[1] = (height - 2.0*y) / height;
	d = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
	v[2] = (float)cos((PI / 2.0)*((d<1.0) ? d : 1.0));
	a = 1.0 / (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}

void mouseMotion(int x, int y)
{
	float curPos[3], dx, dy, dz;
	trackball(x, y, winWidth, winHeight, curPos);
	if (trackingMouse)
	{
		dx = curPos[0] - lastPos[0];
		dy = curPos[1] - lastPos[1];
		dz = curPos[2] - lastPos[2];

		if (dx || dy || dz)
		{

			angle = 90.0 * sqrt(dx * dx + dy * dy + dz * dz);
			axis[0] = lastPos[1] * curPos[2] - lastPos[2] * curPos[1];
			axis[1] = lastPos[2] * curPos[0] - lastPos[0] * curPos[2];
			axis[2] = lastPos[0] * curPos[1] - lastPos[1] * curPos[0];

			lastPos[0] = curPos[0];
			lastPos[1] = curPos[1];
			lastPos[2] = curPos[2];
		}
	}
	glutPostRedisplay();
}

void startMotion(int x, int y)
{
	trackingMouse = true;
	redrawContinue = false;
	startX = x;
	startY = y;
	trackball(x, y, winWidth, winHeight, lastPos);
	trackballMove = true;
}

void stopMotion(int x, int y)
{
	trackingMouse = false;

	if (startX != x || startY != y)
	{
		redrawContinue = true;
	}
	else
	{
		redrawContinue = false;
		trackballMove = false;
	}
}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
		switch (state)
		{
		case GLUT_DOWN:	y = winHeight - y;
			startMotion(x, y);
			break;

		case GLUT_UP:	angle = 0;
			stopMotion(x, y);
			break;
		}
	if (button == 3 && state == GLUT_DOWN && winSize > 1)
	{
			winSize = winSize - 0.5;
			winSizeT -= (float)y / winHeight;
			winSizeB -= (float)(winHeight - y) / winHeight;
			winSizeL -= (float)x / winWidth;
			winSizeR -= (float)(winWidth - x) / winWidth;
			shape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			glutPostRedisplay();
	}
	if (button == 4 && state == GLUT_DOWN && winSize < 30)
	{
		winSize = winSize + 0.5;
		winSizeT += (float)y / winHeight;
		winSizeB += (float)(winHeight - y) / winHeight;
		winSizeL += (float)x / winWidth;
		winSizeR += (float)(winWidth - x) / winWidth;
		shape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		glutPostRedisplay();
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		winSize = 5.0;
		winSizeR = winSizeL = winSizeB = winSizeT = 5.0;
		shape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		glutPostRedisplay();
	}
}

void plotGraph()
{
	glColor3f(0, 0, 0);
	if (dimen == 1)
	{
		if (xa)
		{
			//Check power & plot point in x axis
			if (xpower == 1)
			{
				//x = k										EQ 01	
				singleLinear(constant / xco, 0, 0);
			}
			else if (xpower = 2)
			{
				//x^2 = k									EQ 02	
				singleSquare(constant / xco);
			}
		}
		else if (ya)
		{
			//Check power & plot point in y axis
			if (ypower == 1)
			{
				//y = k										EQ 03	
				singleLinear(0, constant / yco, 0);
			}
			else if (ypower == 2)
			{
				//y^2 = k									EQ 04	
				singleSquare(constant / yco);
			}
		}
		else if (za)
		{
			//Check power & plot point in z axis
			if (zpower == 1)
			{
				//z = k										EQ 05	
				singleLinear(0, 0, constant / zco);
			}
			else if (zpower == 2)
			{
				//z^2 = k									EQ 06	
				singleSquare(constant / zco);
			}
		}
	}
	else if (dimen == 2)
	{
		if (xa && ya)
		{
			//Check power & plot lines in xy plane
			if (xpower == 1 && ypower == 1)
			{
				//ax + by = k								EQ 07	
				doubleLinear();
			}
			else if (xpower == 1 && ypower == 2 || xpower == 2 && ypower == 1)
			{
				//ax + by^2 = k								EQ 08	
				//ax^2 + by = k								EQ 09	
				doubleOneSquared(true, false, false);
			}
			else if (xpower == 2 && ypower == 2)
			{
				//ax^2 + by^2 = k							EQ 10	
				doubleSquared(true, false, false);
			}
		}
		else if (ya && za)
		{
			//Check power & plot lines in yz plane
			if (ypower == 1 && zpower == 1)
			{
				//by + cz = k								EQ 11	
				doubleLinear();
			}
			else if (ypower == 1 && zpower == 2 || ypower == 2 && zpower == 1)
			{
				//by + cz^2 = k								EQ 12	
				//by^2 + cz = k								EQ 13	
				doubleOneSquared(false, true, false);
			}
			else if (ypower == 2 && zpower == 2)
			{
				//by^2 + cz^2 = k							EQ 14	
				doubleSquared(false, true, false);
			}
		}
		else if (xa && za)
		{
			//Check power & plot lines in zx plane
			if (xpower == 1 && zpower == 1)
			{
				//ax + cz = k								EQ 15	
				doubleLinear();
			}
			else if (xpower == 1 && zpower == 2 || xpower == 2 && zpower == 1)
			{
				//ax + cz^2 = k								EQ 16	
				//ax^2 + cz = k								EQ 17	
				doubleOneSquared(false, false, true);
			}
			else if (xpower == 2 && zpower == 2)
			{
				//ax^2 + cz^2 = k							EQ 18	
				doubleSquared(false, false, true);
			}
		}
	}
	else if (dimen == 3)
	{
		//Check power and plot plane in space
		if (xa && ya && za)
		{
			if (xpower == 1 && ypower == 1 && zpower == 1)
			{
				//ax + by + cz = k							EQ 19	
				tripleLinear();
			}
			else if (xpower == 1 && ypower == 1 && zpower == 2)
			{
				//ax + by + cz^2 = k						EQ 20	
				tripleTwoLinearOneSquared(true, false, false);
			}
			else if (xpower == 1 && ypower == 2 && zpower == 1)
			{
				//ax + by^2 + cz = k						EQ 21	
				tripleTwoLinearOneSquared(false, false, true);
			}
			else if (xpower == 2 && ypower == 1 && zpower == 1)
			{
				//ax^2 + by + cz = k						EQ 22	
				tripleTwoLinearOneSquared(false, true, false);
			}
			else if (xpower == 1 && ypower == 2 && zpower == 2)
			{
				// ax + by^2 + cz^2 = k						EQ 23	
				tripleOneLinearTwoSquared(false, true, false);
			}
			else if (xpower == 2 && ypower == 2 && zpower == 1)
			{
				//ax^2 + by^2 + cz = k						EQ 24	
				tripleOneLinearTwoSquared(true, false, false);
			}
			else if (xpower == 2 && ypower == 1 && zpower == 2)
			{
				//ax^2 + by + cz^2 = k						EQ 25	
				tripleOneLinearTwoSquared(false, false, true);
			}
			else if (xpower == 2 && ypower == 2 && zpower == 2)
			{
				//ax^2 + by^2 + cz^2 = k					EQ 26	
				tripleSquared();
			}
		}
	}
}

void displayEquation()
{
	glPushMatrix();
	glColor3f(0, 0, 0);
	glLoadIdentity();
	if (winWidth >= winHeight)
		glRasterPos3f(-winSizeL * winWidth / winHeight, -winSizeB, 8 * winSize);
	else
		glRasterPos3f(-winSizeL, -winSizeB * winHeight / winWidth, 8 * winSize);

	for (int i = 0; equation[i] != '\0'; i++)
	{
		if (equation[i] != ' ')
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, toupper(equation[i]));
	}

	glPopMatrix();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (trackballMove)
		glRotatef(angle, axis[0], axis[1], axis[2]);

	drawGrid();

	plotGraph();

	displayEquation();

	glFlush();
}

void checkCoeff(char *ctemp, int c)
{
	if (((ctemp[0] == '-' || ctemp[0] == '+') && ctemp[1] == '\0') || ctemp[0] == '\0')
	{
		ctemp[c] = '1';
		ctemp[++c] = '\0';
	}
}

void checkPower(char *ptemp, int p)
{
	if (ptemp[0] == '\0')
	{
		ptemp[p] = '1';
		ptemp[++p] = '\0';
	}
}

void breakEquation()
{
	xco = 0, yco = 0, zco = 0, xpower = 0, ypower = 0, zpower = 0, constant = 0;
	xa = false, ya = false, za = false;
	char ctemp[90], ptemp[90], ktemp[90];
	int i, c, k, p;
	bool pmode = false, kmode = false, xmode = false, ymode = false, zmode = false;
	bool xdone = false, ydone = false, zdone = false;
	bool numRead = false;
	int signs = 0, vars = 0;

	invalidEquation = false;

	i = c = k = p = 0;
	ctemp[0] = ptemp[0] = ktemp[0] = '\0';

	while (equation[i] != '\0')
	{
		if (isdigit(equation[i]) || (equation[i] == '.' && !pmode))
		{
			if (pmode == true)
			{
				ptemp[p] = equation[i];
				ptemp[++p] = '\0';
			}
			else if (kmode == true)
			{
				ktemp[k] = equation[i];
				ktemp[++k] = '\0';
			}
			else
			{
				ctemp[c] = equation[i];
				ctemp[++c] = '\0';
			}
			numRead = true;
		}
		else if (isalpha(equation[i]) && !kmode)
		{
			if (pmode)
			{
				invalidEquation = true;
				break;
			}
			if (tolower(equation[i]) == 'x')
			{
				xmode = true;
				ymode = zmode = false;
				vars++;
			}
			else if (tolower(equation[i]) == 'y')
			{
				ymode = true;
				xmode = zmode = false;
				vars++;
			}
			else if (tolower(equation[i]) == 'z')
			{
				zmode = true;
				xmode = ymode = false;
				vars++;
			}
			else
			{
				invalidEquation = true;
				break;
			}
			numRead = false;
		}
		else if (equation[i] == '^')
		{
			if (xmode || ymode || zmode)
			{
				pmode = true;
				kmode = false;
			}
			else
			{
				invalidEquation = true;
				break;
			}
		}
		else if (equation[i] == '-' || equation[i] == '+' || equation[i] == '=')
		{
			signs++;
			if (pmode && !numRead)
			{
				invalidEquation = true;
				break;
			}
			if (xmode == true)
			{
				if (xdone == false)
				{
					checkCoeff(ctemp, c);
					checkPower(ptemp, p);
					sscanf_s(ctemp, "%f", &xco);
					sscanf_s(ptemp, "%d", &xpower);
					xa = true;
					xdone = true;
				}
				else
				{
					invalidEquation = true;
					break;
				}
			}
			else if (ymode == true)
			{
				if (ydone == false)
				{
					checkCoeff(ctemp, c);
					checkPower(ptemp, p);
					sscanf_s(ctemp, "%f", &yco);
					sscanf_s(ptemp, "%d", &ypower);
					ya = true;
					ydone = true;
				}
				else
				{
					invalidEquation = true;
					break;
				}
			}
			else if (zmode == true)
			{
				if (zdone == false)
				{
					checkCoeff(ctemp, c);
					checkPower(ptemp, p);
					sscanf_s(ctemp, "%f", &zco);
					sscanf_s(ptemp, "%d", &zpower);
					za = true;
					zdone = true;
				}
				else
				{
					invalidEquation = true;
					break;
				}
			}
			pmode = false;
			p = k = c = 0;
			if (equation[i] == '=')
			{
				numRead = false;
				if (!kmode)
				{
					kmode = true;
					xmode = ymode = zmode = false;
					i++;
					continue;
				}
				else
				{
					invalidEquation = true;
					break;
				}
			}
			if (kmode == true)
			{
				ktemp[k] = equation[i];
				ktemp[++k] = '\0';
				if (numRead)
				{
					invalidEquation = true;
					break;
				}
			}
			else
			{
				ctemp[c] = equation[i];
				ctemp[++c] = '\0';
			}
			ptemp[p] = '\0';
			numRead = false;
		}
		else if (equation[i] == ' ')
		{
			i++;
			continue;
		}
		else
		{
			invalidEquation = true;
			break;
		}
		i++;
	}

	if (ktemp[0] == '\0' || signs < vars || (!xdone && !ydone && !zdone))
		invalidEquation = true;

	sscanf_s(ktemp, "%f", &constant);
}

void validateEquation()
{
	dimen = 0;
	if (xa)
	{
		if ((xpower == 1 || xpower == 2) && xco != 0)
			dimen++;
		else
			invalidEquation = true;
	}
	if (ya)
	{
		if ((ypower == 1 || ypower == 2) && yco != 0)
			dimen++;
		else
			invalidEquation = true;
	}
	if (za)
	{
		if ((zpower == 1 || zpower == 2) && zco != 0)
			dimen++;
		else
			invalidEquation = true;
	}
}

void solutionCheck()
{
	solutionExist = true;
	if (dimen == 1)
	{
		if ((((xco > 0 && xpower == 2) || (yco > 0 && ypower == 2) || (zco > 0 && zpower == 2)) && constant < 0)
			|| (((xco < 0 && xpower == 2) || (yco < 0 && ypower == 2) || (zco < 0 && zpower == 2)) && constant > 0))
			solutionExist = false;
	}
	else if (dimen == 2)
	{
		if (xa && ya && xpower == 2 && ypower == 2)
		{
			if ((xco < 0 && yco < 0 && constant >= 0) || (xco > 0 && yco > 0 && constant <= 0))
			{
				solutionExist = false;
			}
		}
		else if (ya && za && ypower == 2 && zpower == 2)
		{
			if ((yco < 0 && zco < 0 && constant >= 0) || (yco > 0 && zco > 0 && constant <= 0))
			{
				solutionExist = false;
			}
		}
		else if (za && xa && zpower == 2 && xpower == 2)
		{
			if ((zco < 0 && xco < 0 && constant >= 0) || (zco > 0 && xco > 0 && constant <= 0))
			{
				solutionExist = false;
			}
		}
	}
	else if (dimen == 3)
	{
		if (xa && ya && za && xpower == 2 && ypower == 2 && zpower == 2)
		{
			if ((xco < 0 && yco < 0 && zco < 0 && constant >= 0) ||
				(xco > 0 && yco > 0 && zco > 0 && constant <= 0))
			{
				solutionExist = false;
			}
		}
	}
}

void subMenu(int id)
{
	glLoadIdentity();
	switch (id)
	{
	case 1:
		break;
	case 2:
		glRotatef(180, 0.0, 1.0, 0.0);
		break;
	case 3:
		glRotatef(90, 1.0, 0.0, 0.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		break;
	case 4:
		glRotatef(-90, 1.0, 0.0, 0.0);
		glRotatef(-90, 0.0, 1.0, 0.0);
		break;
	case 5:
		glRotatef(-90, 0.0, 1.0, 0.0);
		glRotatef(-90, 1.0, 0.0, 0.0);
		break;
	case 6:
		glRotatef(90, 0.0, 1.0, 0.0);
		glRotatef(-90, 1.0, 0.0, 0.0);
		break;
	}
	glutPostRedisplay();
}

void menu(int id)
{
	switch (id)
	{
	case 2:
		exit(0);
		break;
	}
}

void addMenu()
{
	int id = glutCreateMenu(subMenu);
	glutAddMenuEntry("+ve z-axis", 1);
	glutAddMenuEntry("-ve z-axis", 2);
	glutAddMenuEntry("+ve y-axis", 3);
	glutAddMenuEntry("-ve y-axis", 4);
	glutAddMenuEntry("+ve x-axis", 5);
	glutAddMenuEntry("-ve x-axis", 6);
	glutCreateMenu(menu);
	glutAddSubMenu("View from..", id);
	glutAddMenuEntry("Quit", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void main(int argc, char **argv)
{
	int i;
	char ch;
	
	printf("General Informations:\n\n");
	printf("Equation format: [-+]? Ax^p [-+] By^q [-+] Cz^r = k,\n");
	printf("where A, B, & C are co-efficients; p, q & r are exponential power and k is constant.\n");
	printf("Eg: \"two x squared minus y squared plus z equals 2\" is represented as: 2x^2 - y^2 + z = 2\n\n");
	printf("Axis Color Convention:\n");
	printf("RED\t-> x-axis\n");
	printf("GREEN\t-> y-axis\n");
	printf("BLUE\t-> z-axis\n\n");
	printf("Mouse Button Functions:\n");
	printf("Left Mouse Click + Drag: Rotate the Graph.\n");
	printf("Scroll Up or Down: Zoom In or Out the pointed region.\n");
	printf("Middle Mouse Click: Reset Clipping Volume to default.\n");
	printf("Right Mouse Click: Display Menu.\n\n");

	printf("Enter an Equation: ");

	while (true)
	{
		i = 0;
		while ((ch = getchar()) != '\n' && i < 99)
			equation[i++] = ch;
		equation[i] = '\0';

		breakEquation();
		if (invalidEquation)
		{
			printf("\nEquation is Invalid, Enter Again: ");
			continue;
		}
		validateEquation();
		if (invalidEquation)
		{
			printf("\nPower can either be 1 or 2 and Coefficeint cannot be zero\nEnter Again: ");
			continue;
		}

		solutionCheck();
		if (!solutionExist)
		{
			printf("\nSolution Does not Exist\n\nTry Again: ");
			continue;
		}

		break;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1024, 576);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Graph Plotter");

	glutDisplayFunc(display);
	glutReshapeFunc(shape);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);

	addMenu();

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutMainLoop();
}