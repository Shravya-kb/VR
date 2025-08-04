#include <GL/glut.h>
#include <cmath>
#include <iostream>

// Boat state
float boatX = -1.2f;
float boatY = 0.1f; // Y-position of the boat's base, aligned with water top
bool boatVisible = true;
float boatMovementSpeed = 0.01f; // Speed for boat animation towards target (for mouse click)
float keyboardBoatSpeed = 0.02f; // Speed for boat movement via keyboard
float targetBoatX = boatX;       // Target X position for boat movement (for mouse click)
bool isBoatMoving = false;       // Flag to indicate if boat is currently animating (for mouse click)

// Sinking animation state
bool isSinking = false;
float sinkSpeed = 0.005f; // Speed at which the boat sinks
bool completelyGone = false; // Flag to indicate if boat has sunk completely

// Zoom state for iceberg
float icebergZoomFactor = 2.0f; // Initial larger zoom for iceberg

// Iceberg position (top-right corner, now on the water)
const float icebergX = 1.2f; // X-coordinate for the center of the iceberg
const float icebergY = 0.1f; // Y-coordinate for the base of the iceberg, aligned with water

// Sun's position (also the light source position)
const float sunLightX = -1.0f;
const float sunLightY = 0.8f;
const float sunLightZ = 0.5f; // Z-coordinate to give it some depth/direction

// Function to draw a circle/filled polygon (for sun - no longer used for clouds)
void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(cx, cy, 0.0f); // Center of circle, now with Z=0
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // Get the current angle
        float x = r * cosf(theta); // Calculate the x component
        float y = r * sinf(theta); // Calculate the y component
        glVertex3f(cx + x, cy + y, 0.0f); // Output vertex, now with Z=0
    }
    glEnd();
}

// Draw the sky with a gradient
void drawSky() {
    // Disable lighting temporarily for background elements that shouldn't be lit
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    // Top (lighter blue)
    glColor3f(0.7f, 0.9f, 1.0f); // Lighter blue at the top
    glVertex3f(-1.5f, 1.0f, -0.9f); // Placed far back in Z
    glVertex3f(1.5f, 1.0f, -0.9f);

    // Bottom (slightly darker blue, above water horizon)
    glColor3f(0.4f, 0.7f, 1.0f); // Darker blue towards the horizon
    glVertex3f(1.5f, 0.1f, -0.9f);
    glVertex3f(-1.5f, 0.1f, -0.9f);
    glEnd();
    glEnable(GL_LIGHTING); // Re-enable lighting
}

// Draw the sun with a glow (visual representation only)
void drawSun() {
    glDisable(GL_LIGHTING); // Sun is a light source, but its visual representation doesn't need to be lit itself
    glPushMatrix();
    glTranslatef(sunLightX, sunLightY, sunLightZ - 0.2f); // Position of the sun visually, slightly behind light source actual Z
    // Draw the glow
    glColor3f(1.0f, 0.7f, 0.0f); // Orange glow
    drawCircle(0.0f, 0.0f, 0.18f, 30);

    // Draw the sun core
    glColor3f(1.0f, 0.9f, 0.0f); // Yellow core
    drawCircle(0.0f, 0.0f, 0.15f, 30);
    glPopMatrix();
    glEnable(GL_LIGHTING); // Re-enable lighting
}

// Draw some fluffy clouds (NOW 3D)
void drawClouds() {
    glEnable(GL_LIGHTING); // Re-enable lighting for clouds as they are now 3D objects
    glColor3f(1.0f, 1.0f, 1.0f); // White color for clouds

    // Cloud 1
    glPushMatrix();
    glTranslatef(0.5f, 0.7f, -0.5f); // Z position for clouds
    glutSolidSphere(0.12, 20, 20); // main sphere
    glPushMatrix();
    glTranslatef(0.08f, 0.05f, 0.03f); // Offset for another part of the cloud
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.07f, 0.03f, -0.02f); // Offset for another part
    glutSolidSphere(0.09, 20, 20);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.02f, -0.05f, 0.05f); // Offset for another part
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();
    glPopMatrix();

    // Cloud 2
    glPushMatrix();
    glTranslatef(-0.3f, 0.85f, -0.6f); // Z position for clouds
    glutSolidSphere(0.1, 20, 20);
    glPushMatrix();
    glTranslatef(-0.06f, -0.02f, 0.01f);
    glutSolidSphere(0.08, 20, 20);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.05f, 0.03f, -0.03f);
    glutSolidSphere(0.09, 20, 20);
    glPopMatrix();
    glPopMatrix();

    // Cloud 3
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -0.4f); // Z position for clouds
    glutSolidSphere(0.07, 20, 20);
    glPushMatrix();
    glTranslatef(0.04f, 0.03f, 0.02f);
    glutSolidSphere(0.06, 20, 20);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.03f, -0.01f, -0.01f);
    glutSolidSphere(0.05, 20, 20);
    glPopMatrix();
    glPopMatrix();
    // No glDisable(GL_LIGHTING) at the end, as clouds are now lit 3D objects
}

// Draw the water layer with subtle waves
void drawWater() {
    glColor3f(0.0f, 0.5f, 1.0f); // Blue color for water (will be modulated by lighting)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f); // Normal pointing out of the screen (for flat surface)
    glVertex3f(-1.5f, -1.0f, 0.0f); // Bottom-left corner of water
    glVertex3f(1.5f, -1.0f, 0.0f);  // Bottom-right corner of water
    glVertex3f(1.5f, 0.1f, 0.0f);   // Top-right corner of water
    glVertex3f(-1.5f, 0.1f, 0.0f);  // Top-left corner of water
    glEnd();

    // Draw subtle waves on top of the water (might look better without lighting or with specific material)
    glDisable(GL_LIGHTING); // Temporarily disable lighting for wave lines
    glColor3f(0.8f, 0.9f, 1.0f); // Light blue/white for waves
    glBegin(GL_LINES);
    // Line 1
    glVertex3f(-1.4f, 0.08f, 0.01f); glVertex3f(-1.2f, 0.08f, 0.01f);
    glVertex3f(-1.1f, 0.06f, 0.01f); glVertex3f(-0.9f, 0.06f, 0.01f);
    // Line 2
    glVertex3f(-0.6f, 0.09f, 0.01f); glVertex3f(-0.4f, 0.09f, 0.01f);
    glVertex3f(-0.3f, 0.07f, 0.01f); glVertex3f(-0.1f, 0.07f, 0.01f);
    // Line 3
    glVertex3f(0.2f, 0.08f, 0.01f); glVertex3f(0.4f, 0.08f, 0.01f);
    glVertex3f(0.5f, 0.06f, 0.01f); glVertex3f(0.7f, 0.06f, 0.01f);
    // Line 4
    glVertex3f(0.9f, 0.09f, 0.01f); glVertex3f(1.1f, 0.09f, 0.01f);
    glEnd();
    glEnable(GL_LIGHTING); // Re-enable lighting
}


// Draw the boat (3D)
void drawBoat() {
    if (!boatVisible && !isSinking && completelyGone) return;
    if (!boatVisible && !isSinking) return; // Hide boat if not visible and not sinking

    glPushMatrix();
    glTranslatef(boatX, boatY, 0.0f); // Translate boat to its current position

    // Define boat hull vertices for a box-like shape
    // Front face (Z = 0.05)
    // Back face (Z = -0.05)
    // Length X: -0.2 to 0.2
    // Height Y: 0.0 to 0.1 (relative to boatY)
    float hullZFront = 0.05f;
    float hullZBack = -0.05f;
    float hullHeight = 0.1f;

    // Boat hull (YELLOW)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for the boat body

    // Top Face (GL_QUADS)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing up in Y
    glVertex3f(-0.2f, hullHeight, hullZFront);
    glVertex3f(0.2f, hullHeight, hullZFront);
    glVertex3f(0.2f, hullHeight, hullZBack);
    glVertex3f(-0.2f, hullHeight, hullZBack);
    glEnd();

    // Bottom Face (GL_QUADS)
    glColor3f(0.7f, 0.7f, 0.0f); // Darker yellow for bottom
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal pointing down in Y
    glVertex3f(-0.2f, 0.0f, hullZBack);
    glVertex3f(0.2f, 0.0f, hullZBack);
    glVertex3f(0.2f, 0.0f, hullZFront);
    glVertex3f(-0.2f, 0.0f, hullZFront);
    glEnd();

    // Front Face (GL_QUADS)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f); // Normal pointing out in Z
    glVertex3f(-0.2f, 0.0f, hullZFront);
    glVertex3f(0.2f, 0.0f, hullZFront);
    glVertex3f(0.2f, hullHeight, hullZFront);
    glVertex3f(-0.2f, hullHeight, hullZFront);
    glEnd();

    // Back Face (GL_QUADS)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f); // Normal pointing in negative Z
    glVertex3f(-0.2f, hullHeight, hullZBack);
    glVertex3f(0.2f, hullHeight, hullZBack);
    glVertex3f(0.2f, 0.0f, hullZBack);
    glVertex3f(-0.2f, 0.0f, hullZBack);
    glEnd();

    // Right Side Face (GL_QUADS)
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f); // Normal pointing right in X
    glVertex3f(0.2f, 0.0f, hullZFront);
    glVertex3f(0.2f, 0.0f, hullZBack);
    glVertex3f(0.2f, hullHeight, hullZBack);
    glVertex3f(0.2f, hullHeight, hullZFront);
    glEnd();

    // Left Side Face (GL_QUADS)
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f); // Normal pointing left in X
    glVertex3f(-0.2f, hullHeight, hullZFront);
    glVertex3f(-0.2f, hullHeight, hullZBack);
    glVertex3f(-0.2f, 0.0f, hullZBack);
    glVertex3f(-0.2f, 0.0f, hullZFront);
    glEnd();


    // Red rudder/fin (3D)
    float rudderZFront = 0.01f; // Thin rudder
    float rudderZBack = -0.01f;
    glColor3f(1.0f, 0.0f, 0.0f);

    // Front face of rudder
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f); // Front face normal
    glVertex3f(-0.2f, 0.1f, rudderZFront);
    glVertex3f(-0.25f, 0.15f, rudderZFront);
    glVertex3f(-0.2f, 0.0f, rudderZFront);
    glEnd();

    // Back face of rudder
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, -1.0f); // Back face normal
    glVertex3f(-0.2f, 0.0f, rudderZBack);
    glVertex3f(-0.25f, 0.15f, rudderZBack);
    glVertex3f(-0.2f, 0.1f, rudderZBack);
    glEnd();

    // Side faces of rudder (using QUADS to connect front and back triangles)
    glBegin(GL_QUADS);
    // Bottom edge face
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal points down (approx)
    glVertex3f(-0.2f, 0.0f, rudderZFront);
    glVertex3f(-0.2f, 0.0f, rudderZBack);
    glVertex3f(-0.25f, 0.15f, rudderZBack); // Connects to top point
    glVertex3f(-0.25f, 0.15f, rudderZFront);
    glEnd();

    glBegin(GL_QUADS);
    // Slanted top-front edge face
    glNormal3f(0.707f, 0.707f, 0.0f); // Normal roughly points up-right
    glVertex3f(-0.2f, 0.1f, rudderZFront);
    glVertex3f(-0.2f, 0.1f, rudderZBack);
    glVertex3f(-0.25f, 0.15f, rudderZBack);
    glVertex3f(-0.25f, 0.15f, rudderZFront);
    glEnd();

    glBegin(GL_QUADS);
    // Vertical back-edge face
    glNormal3f(-1.0f, 0.0f, 0.0f); // Normal points left (approx)
    glVertex3f(-0.2f, 0.0f, rudderZFront);
    glVertex3f(-0.2f, 0.1f, rudderZFront);
    glVertex3f(-0.2f, 0.1f, rudderZBack);
    glVertex3f(-0.2f, 0.0f, rudderZBack);
    glEnd();


    // Gray sail/wing on top (3D)
    float sailZFront = 0.01f; // Thin sail
    float sailZBack = -0.01f;
    glColor3f(0.3f, 0.3f, 0.3f);

    // Front face of sail
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f); // Front face normal
    glVertex3f(-0.05f, 0.1f, sailZFront);
    glVertex3f(0.05f, 0.1f, sailZFront);
    glVertex3f(0.0f, 0.2f, sailZFront);
    glEnd();

    // Back face of sail
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, -1.0f); // Back face normal
    glVertex3f(0.0f, 0.2f, sailZBack);
    glVertex3f(0.05f, 0.1f, sailZBack);
    glVertex3f(-0.05f, 0.1f, sailZBack);
    glEnd();

    // Side faces of sail (using QUADS)
    glBegin(GL_QUADS);
    // Left edge face
    glNormal3f(-0.5f, 0.5f, 0.0f); // Normal points left-up
    glVertex3f(-0.05f, 0.1f, sailZFront);
    glVertex3f(-0.05f, 0.1f, sailZBack);
    glVertex3f(0.0f, 0.2f, sailZBack);
    glVertex3f(0.0f, 0.2f, sailZFront);
    glEnd();

    glBegin(GL_QUADS);
    // Right edge face
    glNormal3f(0.5f, 0.5f, 0.0f); // Normal points right-up
    glVertex3f(0.0f, 0.2f, sailZFront);
    glVertex3f(0.0f, 0.2f, sailZBack);
    glVertex3f(0.05f, 0.1f, sailZBack);
    glVertex3f(0.05f, 0.1f, sailZFront);
    glEnd();

    glBegin(GL_QUADS);
    // Bottom edge face
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal points down
    glVertex3f(-0.05f, 0.1f, sailZFront);
    glVertex3f(0.05f, 0.1f, sailZFront);
    glVertex3f(0.05f, 0.1f, sailZBack);
    glVertex3f(-0.05f, 0.1f, sailZBack);
    glEnd();


    glPopMatrix();
}

// Draw iceberg (3D)
void drawIceberg() {
    glPushMatrix();
    glTranslatef(icebergX, icebergY, 0.0f); // Translate iceberg to its base position
    glScalef(icebergZoomFactor, icebergZoomFactor, 1.0f); // Apply zoom to the iceberg

    glColor3f(0.7f, 0.9f, 1.0f); // Light blue/white color for iceberg

    // Draw iceberg as a 3D prism (simple triangular pyramid)
    // Base triangle on XY plane
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f); // Normal for the top face of the base (facing viewer)
    glVertex3f(-0.1f, 0.0f, 0.05f); // Base-left point (front)
    glVertex3f(0.1f, 0.0f, 0.05f);  // Base-right point (front)
    glVertex3f(0.0f, 0.2f, 0.05f);  // Top point (front)
    glEnd();

    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, -1.0f); // Normal for the back face of the base
    glVertex3f(0.0f, 0.2f, -0.05f); // Top point (back)
    glVertex3f(0.1f, 0.0f, -0.05f);  // Base-right point (back)
    glVertex3f(-0.1f, 0.0f, -0.05f); // Base-left point (back)
    glEnd();

    // Side faces (connecting front and back triangles)
    glBegin(GL_QUADS);
    glNormal3f(-0.8f, 0.0f, 0.5f); // Normal for left slant face
    glVertex3f(-0.1f, 0.0f, 0.05f);
    glVertex3f(0.0f, 0.2f, 0.05f);
    glVertex3f(0.0f, 0.2f, -0.05f);
    glVertex3f(-0.1f, 0.0f, -0.05f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.8f, 0.0f, 0.5f); // Normal for right slant face
    glVertex3f(0.1f, 0.0f, 0.05f);
    glVertex3f(0.1f, 0.0f, -0.05f);
    glVertex3f(0.0f, 0.2f, -0.05f);
    glVertex3f(0.0f, 0.2f, 0.05f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal for bottom face
    glVertex3f(-0.1f, 0.0f, 0.05f);
    glVertex3f(-0.1f, 0.0f, -0.05f);
    glVertex3f(0.1f, 0.0f, -0.05f);
    glVertex3f(0.1f, 0.0f, 0.05f);
    glEnd();


    // Add some cracks/details to the iceberg for more realism
    glDisable(GL_LIGHTING); // Lines don't interact with lighting in the same way
    glColor3f(0.5f, 0.7f, 0.8f); // Slightly darker shade for cracks
    glBegin(GL_LINES);
    glVertex3f(-0.05f, 0.05f, 0.06f); glVertex3f(0.0f, 0.1f, 0.06f);
    glVertex3f(0.05f, 0.05f, 0.06f); glVertex3f(0.0f, 0.1f, 0.06f);
    glVertex3f(0.0f, 0.05f, 0.06f); glVertex3f(0.0f, 0.0f, 0.06f);
    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

// Check collision between boat and iceberg
void checkCollision() {
    // Only check collision if boat is visible and not already sinking
    if (!boatVisible || isSinking) return;

    // Boat's approximate 2D bounding box (from -0.25 to 0.2 in X, 0.0 to 0.2 in Y relative to boatX)
    float boatMinX = boatX - 0.25f;
    float boatMaxX = boatX + 0.2f;
    float boatMinY = boatY + 0.0f; // Base of the boat body
    float boatMaxY = boatY + 0.2f; // Top of the sail

    // Iceberg's approximate 2D bounding box, considering its scale
    float icebergHalfWidth = 0.1f * icebergZoomFactor;
    float icebergHeight = 0.2f * icebergZoomFactor;

    float icebergMinX = icebergX - icebergHalfWidth;
    float icebergMaxX = icebergX + icebergHalfWidth;
    float icebergMinY = icebergY; // Base of iceberg
    float icebergMaxY = icebergY + icebergHeight; // Top of iceberg

    // Check for overlap on both X and Y axes (2D collision for simplicity)
    bool xOverlap = (boatMaxX >= icebergMinX && boatMinX <= icebergMaxX);
    bool yOverlap = (boatMaxY >= icebergMinY && boatMinY <= icebergMaxY);

    if (xOverlap && yOverlap) {
        boatVisible = false; // Boat is no longer "visible" as an active entity
        isBoatMoving = false; // Stop any ongoing movement
        isSinking = true;     // Start sinking animation
        std::cout << "Collision! Boat started sinking." << std::endl; // For debugging
    }
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glLoadIdentity();             // Reset the modelview matrix

    // Set the light position (needs to be done in display before drawing objects)
    GLfloat light_position[] = { sunLightX, sunLightY, sunLightZ, 1.0f }; // Positional light
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Draw scenery elements first (background to foreground based on Z)
    drawSky();
    drawSun(); // This draws the visual sun, the light position is set above
    drawClouds(); // Now 3D
    drawWater();
    drawIceberg(); // Already 3D
    drawBoat();    // Already 3D

    glutSwapBuffers(); // Swap the front and back buffers to display the scene
}

// Timer for animation and updates
void timer(int) {
    if (boatVisible) {
        // Animate boat movement towards targetBoatX (for mouse clicks)
        if (isBoatMoving) {
            float distance = targetBoatX - boatX;
            if (std::abs(distance) > boatMovementSpeed) {
                boatX += (distance > 0 ? 1 : -1) * boatMovementSpeed;
            }
            else {
                boatX = targetBoatX; // Reached target
                isBoatMoving = false;
            }
        }
        checkCollision(); // Check for collision with iceberg
    }
    else if (isSinking) {
        // Animate boat sinking
        boatY -= sinkSpeed;
        if (boatY < -1.0f) { // Sunk below screen
            isSinking = false;
            completelyGone = true;
            std::cout << "Boat has completely sunk." << std::endl;
        }
    }

    glutPostRedisplay(); // Request a redraw of the scene
    glutTimerFunc(16, timer, 0); // Call timer again after 16 milliseconds (approx. 60 FPS)
}

// Keyboard function for zooming iceberg and boat movement, and resetting
void keyboard(unsigned char key, int x, int y) {
    if (key == 's' || key == 'S') { // 'S' for zoom IN (makes iceberg bigger)
        icebergZoomFactor += 0.1f;
        if (icebergZoomFactor > 5.0f) icebergZoomFactor = 5.0f; // Cap max zoom
    }
    else if (key == 'w' || key == 'W') { // 'W' for zoom OUT (makes iceberg smaller)
        icebergZoomFactor -= 0.1f;
        if (icebergZoomFactor < 0.1f) icebergZoomFactor = 0.1f; // Don't allow negative or too small zoom
    }
    else if (key == 13) { // ASCII for Enter key
        // Reset boat state
        boatX = -1.2f;
        boatY = 0.1f;
        boatVisible = true;
        isSinking = false;
        completelyGone = false;
        isBoatMoving = false;
        targetBoatX = boatX;
        std::cout << "Story reset! Boat is back." << std::endl;
    }
    else if (boatVisible && !isSinking) { // Only allow boat movement if visible and not sinking
        if (key == 'd' || key == 'D') {
            boatX += keyboardBoatSpeed; // Move boat forward (right)
            isBoatMoving = false; // Stop any mouse-based target movement
        }
        else if (key == 'a' || key == 'A') {
            boatX -= keyboardBoatSpeed; // Move boat backward (left)
            isBoatMoving = false; // Stop any mouse-based target movement
        }
    }
    glutPostRedisplay(); // Request a redraw
}

// Mouse function for cursor-based boat movement target
void mouseClick(int button, int state, int x, int y) {
    // Only allow movement if the boat is visible and not currently sinking
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && boatVisible && !isSinking) {
        // Convert mouse X coordinate to OpenGL coordinates
        // This is a direct mapping for orthographic projection
        targetBoatX = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 3.0f - 1.5f;
        isBoatMoving = true; // Start boat animation
    }
}

// Initialization
void init() {
    glClearColor(0.8f, 0.9f, 1.0f, 1.0f); // Light sky blue background color (fallback)
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D appearance

    glMatrixMode(GL_PROJECTION);          // Set the matrix mode to projection
    glLoadIdentity();                     // Load identity matrix to projection
    // Using glOrtho for orthographic projection, but with a Z-range for 3D objects
    glOrtho(-1.5f, 1.5f, -1.0f, 1.0f, -1.0f, 1.0f); // x, y, near Z, far Z
    // Objects with Z between -1.0 and 1.0 will be visible

    glMatrixMode(GL_MODELVIEW);           // Switch back to modelview matrix mode

    // --- OpenGL Lighting Setup ---
    glEnable(GL_LIGHTING); // Enable lighting
    glEnable(GL_LIGHT0);   // Enable light source 0

    // Define light properties
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Dim ambient light
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 0.8f, 1.0f }; // Bright yellowish-white diffuse light (sun-like)
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // White specular highlight

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Enable color tracking (so glColor affects material properties)
    glEnable(GL_COLOR_MATERIAL);
    // Specify which material properties are controlled by glColor
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Set a default specular material and shininess for all objects if not specified individually
    GLfloat default_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, default_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f); // A moderate shininess
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Add GLUT_DEPTH for depth testing
    glutInitWindowSize(800, 600); // Set window size
    glutCreateWindow("3D Boat, Clouds, and Iceberg Story - Orthographic View"); // Create a window with the given title

    init(); // Call initialization function

    glutDisplayFunc(display);       // Register display callback function
    glutKeyboardFunc(keyboard);     // Register keyboard callback function
    glutMouseFunc(mouseClick);      // Register mouse click callback function
    glutTimerFunc(0, timer, 0);     // Register timer callback for animation

    glutMainLoop(); // Enter the GLUT event processing loop
    return 0;
}
