// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $

#include <typeinfo>
#include <unordered_map>
#include <math.h>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (vertex_list vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({}) {
   vertex_list vert;
   vertex v1 {width/2, height/2};
   vert.push_back(v1);
   vertex v2 {-width/2, height/2};
   vert.push_back(v2);
   vertex v3 {-width/2, -height/2};
   vert.push_back(v3);
   vertex v4 {width/2, -height/2};
   vert.push_back(v4);
   vertices = vert;
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

diamond::diamond (GLfloat width, GLfloat height): polygon({}) {
   vertex_list vert;
   vertex v1 {0, height/2};
   vert.push_back(v1);
   vertex v2 {-width/2, 0};
   vert.push_back(v2);
   vertex v3 {0, -height/2};
   vert.push_back(v3);
   vertex v4 {width/2, 0};
   vert.push_back(v4);
   vertices = vert;
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

triangle::triangle (vertex_list vertices_): polygon(vertices_) {
   DEBUGF ('c', this);
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat length): triangle ({}) {
   DEBUGF ('c', this);
   vertex_list vert;
   vertex v1 {0, static_cast<GLfloat>(sqrt(3.0) / 3 * length)};
   vert.push_back(v1);
   vertex v2 {-length/2,  static_cast<GLfloat>
                                          (-sqrt(3.0) / 3 * length)};
   vert.push_back(v2);
   vertex v3 {length/2,  static_cast<GLfloat>
                                          (-sqrt(3.0) / 3 * length)};
   vert.push_back(v3);
   vertices = vert;
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   void* font = glut_bitmap_font;
   glLoadIdentity();
   glColor3ubv (color.ubvec);
   glRasterPos2i (center.xpos, center.ypos);
   auto ubytes11 = reinterpret_cast<const GLubyte*>
                 (textdata.c_str());
   glutBitmapString (font, ubytes11);
}

void text::draw_border (const vertex& center, 
               const rgbcolor& color, const float width) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   float PI = 3.1415926;
   int i = (dimension.xpos + dimension.ypos) * 10;
   glColor3ubv (color.ubvec);
   glLoadIdentity();
   glTranslatef(center.xpos, center.ypos, 0);
   glBegin(GL_POLYGON);
   for(int x = 0; x < i; x++)  
       glVertex2f(dimension.xpos * cos(2 * PI / i * x), 
                  dimension.ypos * sin(2 * PI / i * x));  
   glEnd();  
   glFlush();
   glutSwapBuffers();
}

void ellipse::draw_border (const vertex& center, 
                   const rgbcolor& color, const float width) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   float PI = 3.1415926;
   int i = (dimension.xpos + dimension.ypos) * 10;
   glColor3ubv (color.ubvec);
   glLineWidth(width);
   glLoadIdentity();
   glTranslatef(center.xpos, center.ypos, 0);
   glBegin(GL_LINE_LOOP);
   for(int x = 0; x < i; x++)  
       glVertex2f(dimension.xpos * cos(2 * PI / i * x), 
                              dimension.ypos * sin(2 * PI / i * x));  
   glEnd();  
   glFlush();
   glutSwapBuffers();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glColor3ubv (color.ubvec);
   glLoadIdentity();
   glTranslatef(center.xpos, center.ypos, 0);
   glBegin(GL_POLYGON);
   for(unsigned int x = 0; x < vertices.size(); x++)
      glVertex2f(vertices.at(x).xpos,vertices.at(x).ypos);
   glEnd();
   glFlush();
   glutSwapBuffers();
}

void polygon::draw_border (const vertex& center, 
                 const rgbcolor& color, const float width) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glColor3ubv (color.ubvec);
   glLineWidth(width);
   glLoadIdentity();
   glTranslatef(center.xpos, center.ypos, 0);
   glBegin(GL_LINE_LOOP);
   for(unsigned int x = 0; x < vertices.size(); x++)
      glVertex2f(vertices.at(x).xpos,vertices.at(x).ypos);
   glEnd();
   glFlush();
   glutSwapBuffers();
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}
