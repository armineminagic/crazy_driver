// function related to colision of files
//using namespace std;

// checks if the next move is outside of the container
bool outOfContainer(RECT &r1, RECT Container, int direction, int move) {
    //cout << "outOfContainer se koristi " << endl;
	// left
	if(direction == 0 && (r1.left - move < Container.left)){
		r1.right = Container.left + (r1.right - r1.left);
		r1.left = Container.left;
		return true;
	}
	// right
	if(direction == 1 && (r1.right + move > Container.right)){
		r1.left = Container.right - (r1.right - r1.left);
		r1.right = Container.right;
		return true;
	}

	// up
	if(direction == 2 && (r1.top - move < Container.top)) {
		r1.bottom = Container.top + (r1.bottom - r1.top);
		r1.top = Container.top;
		return true;

	}

	// down
	if(direction == 3 && (r1.bottom + move > Container.bottom)) {
		r1.top = Container.bottom - (r1.bottom - r1.top);
		r1.bottom = Container.bottom;
		return true;
	}
	return false;

}


// Returns 1 if the rectangles overlap, 0 otherwise
int do_rectangles_intersect(RECT a, RECT b) {

	if (a.left >= b.right || a.right <= b.left ||
    a.top >= b.bottom || a.bottom <= b.top)
		return 0;
    return 1;
}


// makes sure that the player doesn't pass the wall
void checkWall(RECT &r, RECT obstacle, int d, RECT p) {

	if(do_rectangles_intersect(r, obstacle)){
		// fix for diagonal movement(it took way more longer then it should to figure it out :( )
		if((d == 3 || d == 2) && (p.right <= obstacle.left))
			d = 1;

		if((d == 3 || d == 2) && (p.left >= obstacle.right))
			d = 0;

		// right to left
		if(d == 0 && r.left < obstacle.right) {
			r.right += obstacle.right - r.left;
			r.left = obstacle.right;
		}
		// left to right
		if(d == 1 && r.right > obstacle.left) {
			r.left -= r.right - obstacle.left;
			r.right = obstacle.left;
		}
		// down to up
		if(d == 2 && r.top < obstacle.bottom) {
			r.bottom += obstacle.bottom - r.top;
			r.top = obstacle.bottom;
		}
		// up to down
		if(d == 3 && r.bottom > obstacle.top) {
			r.top -= r.bottom - obstacle.top;
			r.bottom = obstacle.top;
		}
	}

}


// game over
int game_over(HWND hwnd, RECT &r1, RECT &obstacol, RECT DefPos) {
    // cout << "gameover se koristi " << endl;
	if(r1.right > obstacol.left && r1.left < obstacol.right && do_rectangles_intersect(r1, obstacol)){

		KillTimer(hwnd,1); // stop the action

		MessageBox(hwnd, TEXT(LOSE_TXT), TEXT(LOSE_TXT_TITLE), MB_OK);

		SetTimer(hwnd,1,UPDATE_RATE, NULL); 		// start the action

		// we place the player to the start position
		r1 = DefPos;
		// if they happen to collide on the start position move the obstacle out of the screen
		// till it's position is set back to default
		while(do_rectangles_intersect(DefPos, obstacol)) {
			obstacol.left -= 4500;
			obstacol.right -= 4500;

		}

		InvalidateRect(hwnd,NULL,false);
		UpdateWindow(hwnd);
		return 1;
	}
	return 0;
}

