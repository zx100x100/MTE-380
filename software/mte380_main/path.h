#ifdef PATH_H
#define PATH_H

class Segment {
  public:
    Segment();
    virtual double getDist();
    virtual double completed(float x, float y);

  protected:
    completed(float x, float y);
}

class Curve: public Segment {
  public:
    double getDist();
    double giv();
}

class Path{
  public:
    Path(Hms* hms);

  private:

};

#endif
