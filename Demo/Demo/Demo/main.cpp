#define IMM2D_WIDTH 150
#define IMM2D_HEIGHT 100
#define IMM2D_SCALE 5

#define IMM2D_IMPLEMENTATION
#include "immediate2d.h"

void DrawBackground(int blue);
void DrawScene(int enemyY, int enemyEalth, bool PlayerTurn, int playerEalth);
void enemyAnimation(int& enemyY, bool& direction);
bool isInAttacco(int mX, int mY); //trasformarlo in un int che trova quale azione è stata premuta
void DrawAttack(int enemyY);

void Movement(int& PlayerX, int& PlayerY);

void run() {
	//UseDoubleBuffering(true);
	int b = RandomInt(0, 255);
	int enemyY = 40;
	bool movingUP = true, PlayerTurn = true;
	int enemyEalth = 100, playerEalth = 100;
	bool statoDelMouse = false;
	int seconds;
	const int NumAttacchi = 14;

	//battaglia
	while (enemyEalth > 0 && playerEalth > 0) {
		seconds = 100;
		int PlayerX = 75, PlayerY = 75;
		int PreviousX = PlayerX, PreviousY = PlayerY;
		int AttackY = 5;
		int AttackX[NumAttacchi];
		for (int i = 0; i < NumAttacchi; i++) {
			AttackX[i] = RandomInt(1, IMM2D_WIDTH);
		}

		while (PlayerTurn == false) {
			UseDoubleBuffering(true);
			//Disegni:
			DrawBackground(b);
			DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth);
			DrawCircle(PlayerX, PlayerY, 2, Green, LightGreen);

			//movimento del Player
			Movement(PlayerX, PlayerY);
			//Hitbox
			if (PlayerX > 21 && PlayerX < 129) {
				PreviousX = PlayerX;
			}
			else {
				PlayerX = PreviousX;
			}
			if (PlayerY > 51 && PlayerY < 79) {
				PreviousY = PlayerY;
			}
			else {
				PlayerY = PreviousY;
			}

			//attacchi:
			for (int i = 0; i < NumAttacchi; i++) {
				DrawRectangle(AttackX[i], AttackY, 5, 5, White, White);
				if (PlayerY<AttackY + 3 && PlayerY>AttackY - 3) {
					if (PlayerX > AttackX[i] - 3 && PlayerX < AttackX[i] + 3) {
						playerEalth -= 10;
					}
				}
			}

			Present();
			AttackY++;

			seconds--;
			if (seconds <= 0) {
				if (playerEalth>0)
				PlayerTurn = true;
			}
			//Tocchi Finali:
			Wait(22);
			Clear();
		}
		//if (playerEalth>0)
		//{
		//	while (PlayerTurn) {
		//		UseDoubleBuffering(false);
		//		Disegni:
		//		DrawBackground(b);
		//		DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth);

		//		Animazioni:
		//		enemyAnimation(enemyY, movingUP);

		//		Azioni:
		//		if (LeftMousePressed()) {
		//			int mX = MouseX();
		//			int mY = MouseY();
		//			if (statoDelMouse == false) {
		//				Attacco:
		//				if (isInAttacco(mX, mY)) {
		//					DrawAttack(enemyY);
		//					enemyEalth -= 15;
		//					PlayerTurn = false;
		//				}
		//			}
		//			statoDelMouse = true;
		//		}
		//		else {
		//			statoDelMouse = false;
		//		}

		//		Tocchi Finali:
		//		Wait(120);
		//		Clear();
		//	}
		//}
		/*Clear();*/
		while (PlayerTurn) {
			UseDoubleBuffering(false);
			//Disegni:
			DrawBackground(b);
			DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth);

			//Animazioni:
			enemyAnimation(enemyY, movingUP);

			//Azioni:
			if (LeftMousePressed()) {
				int mX = MouseX();
				int mY = MouseY();
				if (statoDelMouse == false) {
					//Attacco:
					if (isInAttacco(mX, mY)) {
						DrawAttack(enemyY);
						enemyEalth -= 15;
						PlayerTurn = false;
					}
				}
				statoDelMouse = true;
			}
			else {
				statoDelMouse = false;
			}

			//Tocchi Finali:
			Wait(120);
			Clear();
		
	}
	DrawBackground(b);
	if (enemyEalth <= 0) {
		DrawString(75, 25, "You Won", "Comic Sans MC", 20, White, true);
	}
	else {
		DrawString(75, 25, "You Died", "Comic Sans MC", 20, White, true);
	}
}

void DrawBackground(int blue)
{
	Color stu;
	for (int i = 0; i < IMM2D_WIDTH; i += 5) {
		for (int j = 0; j < IMM2D_HEIGHT; j += 5) {
			stu = MakeColor(i, j, blue);
			DrawRectangle(i, j, 5, 5, stu, 0U);
		}
	}
}

void DrawScene(int enemyY, int enemyEalth, bool PlayerTurn, int playerEalth)
{
	//Nemico
	DrawCircle(75, enemyY, 20, Red, LightRed);
	DrawCircle(75, enemyY, 7, White, LightGray);
	DrawLine(75, enemyY - 5, 75, enemyY + 5, 1, Black);
	DrawRectangle(25, 3, enemyEalth, 5, Green, 0U);
	DrawRectangle(25, 93, playerEalth, 5, Green, Green);
	if (PlayerTurn) {
		//Attacco
		DrawRectangle(5, 70, 45, 15, Red, LightRed);
		DrawString(5, 69, "Attacca", "Comic Sans MS", 8, LightRed);
		//Oggetti
		DrawRectangle(52, 70, 45, 15, Blue, LightBlue);
		DrawString(51, 69, "Oggetti", "Comic Sans MS", 8, LightBlue);
		//Mosse Speciali
		DrawRectangle(99, 70, 46, 15, Green, LightGreen);
		DrawString(98, 69, "Speciale", "Comic Sans MS", 8, LightGreen);
	}
	else {
		DrawRectangle(20, 50, 110, 30, Black, White);
	}
}

void enemyAnimation(int& enemyY, bool& direction)
{
	if (direction) {
		enemyY--;
		if (enemyY < 30) {
			direction = false;
		}
	}
	else {
		enemyY++;
		if (enemyY > 46) {
			direction = true;
		}
	}
}

bool isInAttacco(int mX, int mY)
{
	if ((mX >= 5 && mX <= 50)&&(mY >= 70 && mY <= 85)) {
		return true;
	}
	return false;
}

void DrawAttack(int enemyY)
{
	Color DarkRed = MakeColor(88, 0, 0);
	for (int i = 0; i < 41; i++) {
		//DrawLine(50, enemyY - 20, 51 + i, enemyY + i, 2, Black);
		DrawPixel(52 + i, enemyY - 20 + i, DarkRed);
		DrawPixel(52 + i, enemyY - 21 + i, DarkRed);
		Wait(5);
	}
	DrawString(75, 0, "15", "Comic Sans MC", 15, Black, true);
	Wait(400);
}

void Movement(int& PlayerX, int& PlayerY)
{
	char Movement = LastKey();
	if (Movement == Up) {
		PlayerY -= 2;
	}
	if (Movement == Down) {
		PlayerY += 2;
	}
	if (Movement == Right) {
		PlayerX += 2;
	}
	if (Movement == Left) {
		PlayerX -= 2;
	}
}
