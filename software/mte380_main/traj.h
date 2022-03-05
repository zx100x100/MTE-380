#ifndef TRAJ_H
#define TRAJ_H

class Segment {
  public:
    virtual float getDist();
    // TODO
    virtual double completed(float x, float y);

  protected:
    // completed(float x, float y);
};

class Curve: public Segment {
  public:
    Curve();
    float getDist();
    /* double giv(); */
};

class Line: public Segment {
  public:
    Line(bool active = false);
    float getDist();
    /* double giv(); */
};

class Traj{
  public:
    Traj();

  private:
    Segment* segments[20];
};

#endif
