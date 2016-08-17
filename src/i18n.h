const char* PLAYER = "PLAYER";
const char* LEVEL_COMPLETED = "Level %d completed!";
const char* TIME_LEFT = "TIME LEFT";
const char* SCORE = "SCORE";
const char* LEVEL = "LEVEL";
const char* LIVES = "LIVES";
const char* COMPLETED = "COMPLETED";
const char* GAME_OVER = "GAME OVER";
const char* PRESS_A_TO_TRY_AGAIN = "Press A to try again";
const char* PRESS_A_TO_CONTINUE = "Press A to continue";
const char* NEW_HIGHSCORE = "New highscore!";
const char* ENTER_YOUR_NAME = "Please enter your name below"; 

const char* ES_PLAYER = "JUGADOR";
const char* ES_LEVEL_COMPLETED = "¡Nivel %d completado!";
const char* ES_TIME_LEFT = "T. REST.";
const char* ES_SCORE = "PUNT.";
const char* ES_LEVEL = "NIVEL";
const char* ES_LIVES = "VIDAS";
const char* ES_COMPLETED = "COMPLETADO";
const char* ES_GAME_OVER = "FIN DEL JUEGO";
const char* ES_PRESS_A_TO_TRY_AGAIN = "Presiona A para reintentar";
const char* ES_PRESS_A_TO_CONTINUE = "Presiona A para continuar";
const char* ES_NEW_HIGHSCORE = "¡Nueva puntuación alta!";
const char* ES_ENTER_YOUR_NAME = "Introduce tu nombre abajo";

const char* FR_PLAYER = "JOUEUR";
const char* FR_LEVEL_COMPLETED = "Niveau %d terminé!";
const char* FR_TIME_LEFT = "T. REST.";
const char* FR_SCORE = "POINT.";
const char* FR_LEVEL = "NIVEAU";
const char* FR_LIVES = "VIES";
const char* FR_COMPLETED = "TERMINÉ";
const char* FR_GAME_OVER = "PARTIE TERMINÉE";
const char* FR_PRESS_A_TO_TRY_AGAIN = "Appuyez sur A pour réessayer";
const char* FR_PRESS_A_TO_CONTINUE = "Appuyez sur A pour continuer";  //¡Nueva puntuación alta! Introduce tu nombre abajo  Nouveau score élevé!  Entrez votre nom ci-dessous
const char* FR_NEW_HIGHSCORE = "Nouveau score élevé!";
const char* FR_ENTER_YOUR_NAME = "Entrez votre nom ci-dessous"; 

void setLanguage(u8 languageCode)
{
	switch(languageCode)
	{
		case 5:
			PLAYER = ES_PLAYER;
			LEVEL_COMPLETED = ES_LEVEL_COMPLETED;
			TIME_LEFT = ES_TIME_LEFT;
			SCORE = ES_SCORE;
			LEVEL = ES_LEVEL;
			LIVES = ES_LIVES;
			COMPLETED = ES_COMPLETED;
			GAME_OVER = ES_GAME_OVER;
			PRESS_A_TO_TRY_AGAIN = ES_PRESS_A_TO_TRY_AGAIN;
			PRESS_A_TO_CONTINUE = ES_PRESS_A_TO_CONTINUE;
			NEW_HIGHSCORE = ES_NEW_HIGHSCORE;
			ENTER_YOUR_NAME = ES_ENTER_YOUR_NAME;
			break;
		case 2:
			PLAYER = FR_PLAYER;
			LEVEL_COMPLETED = FR_LEVEL_COMPLETED;
			TIME_LEFT = FR_TIME_LEFT;
			SCORE = FR_SCORE;
			LEVEL = FR_LEVEL;
			LIVES = FR_LIVES;
			COMPLETED = FR_COMPLETED;
			GAME_OVER = FR_GAME_OVER;
			PRESS_A_TO_TRY_AGAIN = FR_PRESS_A_TO_TRY_AGAIN;
			PRESS_A_TO_CONTINUE = FR_PRESS_A_TO_CONTINUE;
			NEW_HIGHSCORE = FR_NEW_HIGHSCORE;
			ENTER_YOUR_NAME = FR_ENTER_YOUR_NAME;
			break;
	}
}


const char* i18nPlayer(){ return PLAYER; };
const char* i18nLevelCompleted(){ return LEVEL_COMPLETED; };
const char* i18nTimeLeft(){ return TIME_LEFT; };
const char* i18nScore(){ return SCORE; };
const char* i18nLevel(){ return LEVEL; };
const char* i18nLives(){ return LIVES; };
const char* i18nCompleted(){ return COMPLETED; };
const char* i18nGameOver(){ return GAME_OVER; };
const char* i18nPressAToTryAgain(){ return PRESS_A_TO_TRY_AGAIN; };
const char* i18nPressAToContinue(){ return PRESS_A_TO_CONTINUE; };
const char* i18nNewHighscore(){ return NEW_HIGHSCORE; };
const char* i18nEnterYourName(){ return ENTER_YOUR_NAME; };


