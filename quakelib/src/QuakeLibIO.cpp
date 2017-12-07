// Copyright (c) 2012-2014 Eric M. Heien
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include "QuakeLibIO.h"
#include "QuakeLibEQSim.h"

quakelib::ModelFault &quakelib::ModelWorld::fault(const UIndex &ind) throw(std::domain_error) {
    std::map<UIndex, ModelFault>::iterator it = _faults.find(ind);

    if (it == _faults.end()) throw std::domain_error("quakelib::ModelWorld::fault");
    else return it->second;
}

quakelib::ModelSection &quakelib::ModelWorld::section(const UIndex &ind) throw(std::domain_error) {
    std::map<UIndex, ModelSection>::iterator it = _sections.find(ind);

    if (it == _sections.end()) throw std::domain_error("quakelib::ModelWorld::section");
    else return it->second;
}

quakelib::ModelElement &quakelib::ModelWorld::element(const UIndex &ind) throw(std::domain_error) {
    std::map<UIndex, ModelElement>::iterator it = _elements.find(ind);

    if (it == _elements.end()) throw std::domain_error("quakelib::ModelWorld::element");
    else return it->second;
}

quakelib::ModelVertex &quakelib::ModelWorld::vertex(const UIndex &ind) throw(std::domain_error) {
    std::map<UIndex, ModelVertex>::iterator it = _vertices.find(ind);

    if (it == _vertices.end()) throw std::domain_error("quakelib::ModelWorld::vertex");
    else return it->second;
}

std::string quakelib::ModelIO::next_line(std::istream &in_stream) {
    std::string line = "";
    size_t      pos;

    do {
        std::getline(in_stream, line);
        _comment = "";
        // Cut off any initial whitespace
        pos = line.find_first_not_of(" \t");

        if (pos != std::string::npos) line = line.substr(pos, std::string::npos);

        // Comment consists of hash mark until the end of the line
        pos = line.find("#");

        if (pos != std::string::npos) _comment = line.substr(pos, std::string::npos);

        // Extract the non-comment part of the line
        line = line.substr(0, line.find("#"));

        // If the line is empty, we keep going
        if (line.length() > 0) break;
    } while (in_stream && !in_stream.eof());

    return line;
}

void quakelib::ModelIO::next_line(std::ostream &out_stream) const {
    if (!_comment.empty()) out_stream << " # " << _comment;

    out_stream << "\n";
}

void quakelib::ModelSection::get_field_descs(std::vector<FieldDesc> &descs) {
    FieldDesc       field_desc;
#ifdef HDF5_FOUND
    hid_t           section_name_datatype;

    // Create the datatype for the section name strings
    section_name_datatype = H5Tcopy(H5T_C_S1);
    H5Tset_size(section_name_datatype, (size_t)NAME_MAX_LEN);
#endif

    field_desc.name = "id";
    field_desc.details = "Unique ID of the section.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SectionData, _id);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "fault_id";
    field_desc.details = "ID of the parent fault.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SectionData, _fault_id);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "name";
    field_desc.details = "Name of the section.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SectionData, _name);
    field_desc.type = section_name_datatype;
    field_desc.size = sizeof(char)*NAME_MAX_LEN;
#endif
    descs.push_back(field_desc);

    // TODO: handle this release properly
    //H5Tclose(section_name_datatype);
}

void quakelib::ModelFault::get_field_descs(std::vector<FieldDesc> &descs) {
    FieldDesc       field_desc;
#ifdef HDF5_FOUND
    hid_t           fault_name_datatype;

    // Create the datatype for the fault name strings
    fault_name_datatype = H5Tcopy(H5T_C_S1);
    H5Tset_size(fault_name_datatype, (size_t)NAME_MAX_LEN);
#endif

    field_desc.name = "id";
    field_desc.details = "Unique ID of the fault.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(FaultData, _id);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "fault_name";
    field_desc.details = "Name of the Fault.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(FaultData, _name);
    field_desc.type = fault_name_datatype;
    field_desc.size = sizeof(char)*NAME_MAX_LEN;
#endif
    descs.push_back(field_desc);

    field_desc.name = "fault_area";
    field_desc.details = "Area of the fault [m^2].";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(FaultData, _area);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "fault_length";
    field_desc.details = "Length of the fault [m].";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(FaultData, _length);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);
}


void quakelib::ModelSection::read_data(const SectionData &in_data) {
    memcpy(&_data, &in_data, sizeof(SectionData));
}

void quakelib::ModelSection::write_data(SectionData &out_data) const {
    memcpy(&out_data, &_data, sizeof(SectionData));
}

void quakelib::ModelFault::read_data(const FaultData &in_data) {
    memcpy(&_data, &in_data, sizeof(FaultData));
}

void quakelib::ModelFault::write_data(FaultData &out_data) const {
    memcpy(&out_data, &_data, sizeof(FaultData));
}

void quakelib::ModelSection::read_ascii(std::istream &in_stream) {
    std::stringstream   ss(next_line(in_stream));

    ss >> _data._id;
    ss >> _data._fault_id;
    ss >> _data._name;
}

void quakelib::ModelFault::read_ascii(std::istream &in_stream) {
    std::stringstream   ss(next_line(in_stream));
    ss >> _data._id;
    ss >> _data._name;
    ss >> _data._area;
    ss >> _data._length;
}

void quakelib::ModelSection::write_ascii(std::ostream &out_stream) const {
    out_stream << _data._id << " ";
    out_stream << _data._fault_id << " ";
    out_stream << _data._name;

    next_line(out_stream);
}

void quakelib::ModelFault::write_ascii(std::ostream &out_stream) const {
    out_stream << _data._id << " ";
    out_stream << _data._name << " ";
    out_stream << _data._area << " ";
    out_stream << _data._length;

    next_line(out_stream);
}

void quakelib::ModelElement::get_field_descs(std::vector<FieldDesc> &descs) {
    FieldDesc       field_desc;

    field_desc.name = "id";
    field_desc.details = "Unique ID of the element.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _id);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "section_id";
    field_desc.details = "ID of the section associated with the element.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _section_id);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "vertex_0";
    field_desc.details = "ID of vertex 0.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _vertices[0]);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "vertex_1";
    field_desc.details = "ID of vertex 1.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _vertices[1]);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "vertex_2";
    field_desc.details = "ID of vertex 2.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _vertices[2]);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "is_quad";
    field_desc.details = "Whether the vertices constitute 3 points of a triangle (zero) or 3 points of a parallelogram (non-zero).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _is_quad);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "slip_rate";
    field_desc.details = "Long term slip rate of element in meters per second.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _slip_rate);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "aseismic";
    field_desc.details = "Fraction of slip on element that is aseismic.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _aseismic);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "rake";
    field_desc.details = "Rake angle of element in radians.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _rake);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "lame_mu";
    field_desc.details = "Lame's parameter describing the shear modulus of the material for this element (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _lame_mu);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "lame_lambda";
    field_desc.details = "Lame's lambda parameter of the material for this element, in Pascals.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _lame_lambda);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "max_slip";
    field_desc.details = "Maximum slip distance for this element, in meters.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _max_slip);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "stress_drop";
    field_desc.details = "Stress drop for this element, in Pascals.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(ElementData, _stress_drop);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

}

void quakelib::ModelElement::read_data(const ElementData &in_data) {
    memcpy(&_data, &in_data, sizeof(ElementData));
}

void quakelib::ModelElement::write_data(ElementData &out_data) const {
    memcpy(&out_data, &_data, sizeof(ElementData));
}

void quakelib::ModelElement::read_ascii(std::istream &in_stream) {
    unsigned int        i;
    std::stringstream   ss(next_line(in_stream));

    ss >> _data._id;
    ss >> _data._section_id;

    for (i=0; i<3; ++i) ss >> _data._vertices[i];

    ss >> _data._is_quad;
    ss >> _data._slip_rate;
    ss >> _data._aseismic;
    ss >> _data._rake;
    ss >> _data._lame_mu;
    ss >> _data._lame_lambda;
    ss >> _data._max_slip;
    ss >> _data._stress_drop;
}

void quakelib::ModelElement::write_ascii(std::ostream &out_stream) const {
    unsigned int        i;

    out_stream << _data._id << " ";
    out_stream << _data._section_id << " ";

    for (i=0; i<3; ++i) out_stream << _data._vertices[i] << " ";

    out_stream << _data._is_quad << " ";
    out_stream << _data._slip_rate << " ";
    out_stream << _data._aseismic << " ";
    out_stream << _data._rake << " ";
    out_stream << _data._lame_mu << " ";
    out_stream << _data._lame_lambda << " ";
    out_stream << _data._max_slip << " ";
    out_stream << _data._stress_drop;

    next_line(out_stream);
}

void quakelib::FaultTracePoint::read_ascii(std::istream &in_stream) {
    float      lat, lon, alt;
    std::stringstream   ss(next_line(in_stream));

    ss >> lat;
    ss >> lon;
    ss >> alt;
    _pos = LatLonDepth(lat, lon, alt);
    ss >> _depth_along_dip;
    ss >> _slip_rate;
    ss >> _aseismic;
    ss >> _rake;
    ss >> _dip;
    ss >> _lame_mu;
    ss >> _lame_lambda;
}

void quakelib::FaultTracePoint::write_ascii(std::ostream &out_stream) const {
    out_stream << _pos.lat() << " ";
    out_stream << _pos.lon() << " ";
    out_stream << _pos.altitude() << " ";
    out_stream << _depth_along_dip << " ";
    out_stream << _slip_rate << " ";
    out_stream << _aseismic << " ";
    out_stream << _rake << " ";
    out_stream << _dip << " ";
    out_stream << _lame_mu << " ";
    out_stream << _lame_lambda;

    next_line(out_stream);
}

/*
 Provides a spline to help mesh faults along trace points. This spline is referenced by t (in [0,1])
 representing how long along the trace a point is. The spline is a linear interpolation between successive points.
 */
class TraceSpline {
    private:
        //! The points comprising the spline
        std::vector<quakelib::Vec<3> >  _pts;
        //! Total length of the distances between successive points
        double                          _spline_len;
        //! Individual lengths between successive points
        //! _point_dists[i] is the distance between _pts[i] and _pts[i+1]
        std::vector<double>             _point_dists;

    public:
        TraceSpline(void) : _spline_len(0) {};

        // Add another point to the spline
        void add_point(const quakelib::Vec<3> &new_pt) {
            double add_dist = (_pts.size() > 0 ? _pts.back().dist(new_pt) : 0);
            _spline_len += add_dist;

            if (_pts.size() > 0) _point_dists.push_back(add_dist);

            _pts.push_back(new_pt);
        }

        // Return the element index and inner t corresponding to parameter t
        void get_element(const double t, unsigned int &index, double &inner_t) {
            double spline_dist = t * _spline_len;

            // Ensure t is in [0,1]
            assertThrow(t >= 0 && t <= 1, std::out_of_range("TraceSpline::interpolate"));

            // Go through each point
            for (unsigned int i=0; i<_pts.size()-1; ++i) {
                // If we're between the points for this t, interpolate and return
                if (spline_dist <= _point_dists[i]) {
                    index = i;

                    if (_point_dists[i] != 0) inner_t = spline_dist / _point_dists[i];
                    else inner_t = 0;

                    return;
                }

                spline_dist -= _point_dists[i];
            }

            // If we reach the end, we return the final point and inner_t = 0
            index = _pts.size()-1;
            inner_t = 0;
        }

        // Return the point on this spline at t
        quakelib::Vec<3> interpolate(const double t) {
            unsigned int    ind;
            double          inner_t;

            assertThrow(t >= 0 && t <= 1, std::out_of_range("TraceSpline::interpolate"));
            get_element(t, ind, inner_t);

            if (ind == _pts.size() - 1) return _pts.back();
            else return _pts.at(ind) * (1-inner_t) + _pts.at(ind+1) * inner_t;
        }

        // Given a starting point t (in [0,1]) and an element size,
        // returns the next t which is linear elem_size away (not distance along the spline)
        double advance_element(const double t, const double elem_size) {
            unsigned int    ind;
            double          inner_t;

            // Find the starting point on the spline at t
            quakelib::Vec<3> start_pt = interpolate(t);

            // Get the element that the starting point is associated iwth
            get_element(t, ind, inner_t);

            // Keep going until we find a trace point which would create
            // an element greater than our target size
            double cur_dist = t * _spline_len;

            /*// Wilson: Correcting indexing for horizontal size
            while (ind+1 < _pts.size() && start_pt.dist(_pts.at(ind+1)) < elem_size) {
                ind++;

                if (ind < _point_dists.size()) cur_dist += (1-inner_t) * _point_dists.at(ind);
                else cur_dist += elem_size;

                inner_t = 0;
            }*/
            while (ind < _pts.size()-1 && start_pt.dist(_pts.at(ind+1)) < elem_size) {

                if (ind < _point_dists.size()-1) cur_dist += (1-inner_t) * _point_dists.at(ind);
                else cur_dist = (t * _spline_len) + elem_size;

                ind++;
                inner_t = 0;
            }

            // If we're past the end of the trace, return our best guess
            // for t based on the size of the last segment
            // This is needed to adjust the element size during meshing
            if (ind+1 == _pts.size()) return cur_dist/_spline_len;

            // Now we know the points between which our element must exist (ind and ind+1)
            double      x_0 = _pts.at(ind)[0], x_1 = _pts.at(ind+1)[0], x_s = start_pt[0];
            double      y_0 = _pts.at(ind)[1], y_1 = _pts.at(ind+1)[1], y_s = start_pt[1], l = elem_size;

            // Calculate the inner t between these two points
            double next_t = fabs((sqrt(pow(-2*x_0*x_0+2*x_0*x_1+2*x_0*x_s-2*x_1*x_s-2*y_0*y_0+2*y_0*y_1+2*y_0*y_s-2*y_1*y_s, 2)-
                                       4*(x_0*x_0-2*x_0*x_1+x_1*x_1+y_0*y_0-2*y_0*y_1+y_1*y_1)*
                                       (x_0*x_0-2*x_0*x_s+x_s*x_s+y_0*y_0-2*y_0*y_s+y_s*y_s-l*l))+
                                  2*x_0*x_0-2*x_0*x_1-2*x_0*x_s+2*x_1*x_s+2*y_0*y_0-2*y_0*y_1-2*y_0*y_s+2*y_1*y_s)/
                                 (2*(x_0*x_0-2*x_0*x_1+x_1*x_1+y_0*y_0-2*y_0*y_1+y_1*y_1)));

            // Given this point, recalculate t and return
            cur_dist += (next_t-inner_t) * _point_dists[ind];
            return cur_dist/_spline_len;
        };
};

void quakelib::ModelWorld::create_section(std::vector<unsigned int> &unused_trace_segments, const std::vector<FaultTracePoint> &trace, const LatLonDepth &base_coord, const UIndex &fault_id, const float &element_size, const std::string &section_name, const std::string &taper_method, const bool resize_trace_elements, const UIndex &sec_id) {
    Vec<3>              cur_trace_point, next_trace_point, element_end, element_step_vec, vert_step, vert_ele_vec, full_trace_vec;
    std::vector<UIndex> elem_ids;
    std::set<unsigned int> unused_trace_pts;
    double              elem_depth, elem_slip_rate, elem_aseismic;
    double              elem_rake, elem_dip;
    double              elem_lame_mu, elem_lame_lambda;
    unsigned int        num_vert_elems, ve, elem_count;
    double              taper_t;
    Conversion          conv(base_coord);
    unsigned int        i, num_trace_pts;
    TraceSpline         spline;

    if (element_size <= 0) return;

    num_trace_pts = trace.size();
    full_trace_vec = conv.convert2xyz(trace.back().pos()) - conv.convert2xyz(trace.front().pos());

    if (num_trace_pts == 0 || num_trace_pts == 1) return;

    ModelSection &section = new_section();
    section.set_name(section_name);
    section.set_fault_id(fault_id);

    if (sec_id != -1) section.set_id(sec_id);

    // Create a spline with the trace points
    for (i=0; i<num_trace_pts; ++i) {
        Vec<3> pt = conv.convert2xyz(trace.at(i).pos());
        //Vec<3> pt = conv.yxz2xyz(trace.at(i).pos()); //Use for importing trace in (y, x) halfspace coords.  Must turn off LatLonDepth error throwning for angles out of bounds in QuakeLibUtil.h.
        spline.add_point(pt);
        unused_trace_pts.insert(i);
    }

    // Initially we don't know the appropriate element size to exactly mesh
    // the trace points. This is solved by starting with the user suggested
    // element size and slowly shrinking it down to half the original element size until the mesh fits the
    // trace exactly.
    double cur_elem_size_guess = element_size;
    double step_size = element_size/(2*1000);
    double best_step = element_size, best_t = 0;
    unsigned int best_elem_count = 0;

    while (cur_elem_size_guess > element_size/2.0) {
        double cur_t, next_t, sum_t=0;
        elem_count = 0;
        cur_t = 0;

        while (cur_t < 1) {
            next_t = spline.advance_element(cur_t, cur_elem_size_guess);

            //Wilson: changed this condition to <= to get last column of elements for perfect fit traces
            if (next_t <= 1) {
                sum_t += next_t-cur_t;
                elem_count++;
                cur_t = next_t;
            } else {
                break;
            }
        }

        // If we used a fixed element size, one time through is enough
        if (!resize_trace_elements) {
            best_step = cur_elem_size_guess;
            best_elem_count = elem_count;
            break;
        }

        // Record which element size got us closest to the end of the trace
        if (cur_t > best_t) {
            best_t = cur_t;
            best_step = cur_elem_size_guess;
            best_elem_count = elem_count;
        }

        cur_elem_size_guess -= step_size;
    }

    double  horiz_elem_size = best_step;
    double  vert_elem_size;
	double  horiz_vert_angle, vert_ele_angle;
    unsigned int cur_elem_ind, next_elem_ind;
    double cur_t = 0, next_t;

    double dist_along_strike = 0;
    double taper_flow = 0;
    double taper_full = 0;
    double fault_area = 0;
    cur_trace_point = spline.interpolate(cur_t);
    cur_elem_ind = 0;
    unused_trace_pts.erase(0);

    for (i=0; i<best_elem_count; ++i) {
        // Get the next t value along the trace by advancing the element size
        next_t = spline.advance_element(cur_t, horiz_elem_size);

        // If we go past the end of the spline, align ourselves with the end
        if (next_t > 1) next_t = 1;

        // And get the actual point corresponding to this t
        next_trace_point = spline.interpolate(next_t);

        double          inner_t;
        // And the element and inner_t corresponding to it
        spline.get_element(cur_t, cur_elem_ind, inner_t);

        next_elem_ind = cur_elem_ind + 1;

        // Mark the element corresponding to this point as having been used
        unused_trace_pts.erase(next_elem_ind);

        element_step_vec = next_trace_point-cur_trace_point;

        // Use the t value between the trace points for interpolation
        elem_depth = inner_t *trace.at(next_elem_ind).depth_along_dip()+(1.0-inner_t)*trace.at(cur_elem_ind).depth_along_dip();


        elem_slip_rate = conv.cm_per_yr2m_per_sec(inner_t *trace.at(next_elem_ind).slip_rate()+(1.0-inner_t)*trace.at(cur_elem_ind).slip_rate());

        elem_aseismic = inner_t *trace.at(next_elem_ind).aseismic()+(1.0-inner_t)*trace.at(cur_elem_ind).aseismic();

        elem_dip = conv.deg2rad(inner_t *trace.at(next_elem_ind).dip()+(1.0-inner_t)*trace.at(cur_elem_ind).dip());

        elem_rake = conv.deg2rad(inner_t *trace.at(next_elem_ind).rake()+(1.0-inner_t)*trace.at(cur_elem_ind).rake());

        elem_lame_mu = inner_t *trace.at(next_elem_ind).lame_mu()+(1.0-inner_t)*trace.at(cur_elem_ind).lame_mu();

        elem_lame_lambda = inner_t *trace.at(next_elem_ind).lame_lambda()+(1.0-inner_t)*trace.at(cur_elem_ind).lame_lambda();

        // Ensure square elements, vertical number for best fit.
		vert_elem_size = horiz_elem_size;

        // Set up the vertical vector along the elements.
		//      (Wilson: we no longer extrude straight columns as this misbehaved on shallow dipping curved faults)
		// 		vert_step is the vector perpendicular to the overall trace along which elements are placed,
		//                which obeys fault dip angle
		//      vert_ele_vec is the vector between the top of each element and their bottom vertex,
		//                    the angle of which is solved to maintain overall dip
        ///// Schultz: We need a right handed convention. This complies with geological convention.
        //    RIGHT HANDED CONVENTION
        vert_step = full_trace_vec.rotate_around_axis(Vec<3>(0,0,-1), M_PI/2);
        vert_step = vert_step.rotate_around_axis(full_trace_vec, M_PI-elem_dip);
        vert_step = vert_step.unit_vector();

        horiz_vert_angle = vert_step.vector_angle(element_step_vec);

        vert_step = vert_step*vert_elem_size/sin(horiz_vert_angle);

        vert_ele_angle = asin(vert_step[2]/vert_elem_size);
        vert_ele_vec = element_step_vec.rotate_around_axis(Vec<3>(0,0,-1), M_PI/2);
		vert_ele_vec = vert_ele_vec.rotate_around_axis(element_step_vec, M_PI+vert_ele_angle);

        // Wilson: New column generation can put squares at angles relative to dip; use the actual length
        //          of the element along the dip direction to determine number
        // num_vert_elems = round(elem_depth/vert_elem_size);
        num_vert_elems = round(elem_depth/vert_step.mag());
        // TODO: change this to an assertion throw
		if (num_vert_elems == 0) std::cerr << "WARNING: Depth is smaller than element size in trace segment "
											   << next_elem_ind << ". Element size may be too big." << std::endl;


        // Create each of the elements along dip
        for (ve=0; ve<num_vert_elems; ++ve) {
            // Calculate values for element based on interpolation between trace points
            taper_t = 1;

            //Wilson: Horizontal tapering is now in ModelWorld::create_faults.

            //Vertical tapering with sqrt of depth
            if (taper_method == "taper" || taper_method == "taper_full" || taper_method == "taper_renorm") {

                double z = (float(ve)+0.5)/num_vert_elems;
                taper_t *= sqrt(1-z);
            }

            taper_flow += taper_t *elem_slip_rate*(horiz_elem_size*vert_elem_size);
            taper_full += elem_slip_rate*(horiz_elem_size*vert_elem_size);

            // Create the new vertices
            ModelVertex &v0 = new_vertex();
            ModelVertex &v1 = new_vertex();
            ModelVertex &v2 = new_vertex();

            // Set xyz for the vertices
            v0.set_xyz(cur_trace_point+vert_step*ve, base_coord);
            v1.set_xyz(v0.xyz()+vert_ele_vec, base_coord);
            v2.set_xyz(v0.xyz()+element_step_vec, base_coord);

            //v1.set_xyz(cur_trace_point+vert_step*(ve+1), base_coord);
            //v2.set_xyz(cur_trace_point+vert_step*ve+element_step_vec, base_coord);

            // If this element is at the top, it is a trace point.
            // Keep track for field plots with PyVQ
            if (ve == 0) {
                v0.set_is_trace(1);
                v2.set_is_trace(1);
            }

            // Set distance along strike for each vertex
            v0.set_das(dist_along_strike);
            v1.set_das(dist_along_strike);
            v2.set_das(dist_along_strike+horiz_elem_size);

            // Create element and set up with the created vertices and values
            ModelElement &elem = new_element();
            elem_ids.push_back(elem.id());
            elem.set_section_id(section.id());
            elem.set_vertex(0, v0.id());
            elem.set_vertex(1, v1.id());
            elem.set_vertex(2, v2.id());
            elem.set_is_quad(true);
            elem.set_slip_rate(elem_slip_rate*taper_t);
            elem.set_aseismic(elem_aseismic);
            elem.set_rake(elem_rake);
            elem.set_lame_mu(elem_lame_mu);
            elem.set_lame_lambda(elem_lame_lambda);

            fault_area += horiz_elem_size*vert_elem_size;
        }

        cur_t = next_t;
        dist_along_strike += horiz_elem_size;
        cur_trace_point = next_trace_point;
    }

    if (taper_method == "taper_renorm") {
        double renorm_factor = taper_full/taper_flow, cur_slip_rate;

        for (unsigned int i=0; i<elem_ids.size(); ++i) {
            cur_slip_rate = element(elem_ids[i]).slip_rate();
            element(elem_ids[i]).set_slip_rate(renorm_factor*cur_slip_rate);
        }
    }

    // Go through the created elements and assign maximum slip based on the total fault area
    // Schultz: Updating these to a newer paper. From Leonard 2010
    double moment_magnitude = 4.0+log10(conv.sqm2sqkm(fault_area));

    for (unsigned int i=0; i<elem_ids.size(); ++i) {
        double max_slip = pow(10, (3.0/2.0)*(moment_magnitude+6.0))/(element(elem_ids[i]).lame_mu()*fault_area);
        element(elem_ids[i]).set_max_slip(max_slip);
    }
}

void quakelib::ModelWorld::create_faults_minimal(void) {
    std::map<UIndex, ModelSection>::const_iterator sit;

    // populate _faults with data
    for (sit=_sections.begin(); sit!=_sections.end(); ++sit) {

        if (_faults.count(sit->second.fault_id())==0) {
            ModelFault &fault = new_fault(sit->second.fault_id());
            fault.set_name(sit->second.name()); // Currently uses first section's name as fault name
            fault.insert_section_id(sit->first);
            fault.set_length(0.0);
            fault.set_area(0.0);
        } else {
            ModelFault &fault = _faults[sit->second.fault_id()];
            fault.insert_section_id(sit->first);
            fault.set_length(0.0);
            fault.set_area(0.0);
        }
    }

}




void quakelib::ModelWorld::create_faults(const std::string &taper_method, const bool &vertDASbysec) {
    std::map<UIndex, ModelFault>::const_iterator fit;
    std::map<UIndex, ModelSection>::const_iterator sit;
    std::map<UIndex, ModelElement>::iterator eit;
    std::map<UIndex, ModelVertex>::iterator vit;
    std::map<UIndex, UIndex> vertSects;
    std::map<UIndex, float> sectLengths;
    std::map<UIndex, float> sectStartDAS;

    int i;
    float DAStotal, faultlength, eldas, innerdist, newSlipRate;
    float sectionlength, taper_factor, renorm_factor;
    UIndex sid, fid;
    ElementIDSet sec_IDs;
    ElementIDSet::iterator sidit;
    SimElement simElem;

    // populate _faults with data
    for (sit=_sections.begin(); sit!=_sections.end(); ++sit) {

        sectionlength = section_length(sit->first);
        sectLengths[sit->first] = sectionlength;
        double sectionarea = section_area(sit->first);

        if (_faults.count(sit->second.fault_id())==0) {
            ModelFault &fault = new_fault(sit->second.fault_id());
            fault.set_name(sit->second.name()); // Currently uses first section's name as fault name
            fault.set_length(sectionlength);
            fault.set_area(sectionarea);
            fault.insert_section_id(sit->first);
        } else {
            ModelFault &fault = _faults[sit->second.fault_id()];
            fault.set_length(fault.length()+sectionlength);
            fault.set_area(fault.area()+sectionarea);
            fault.insert_section_id(sit->first);
        }
    }

    // record which vertices belong to which sections
    for (eit=_elements.begin(); eit!=_elements.end(); eit++) {
        for (i=0; i<3; i++) {
            vertSects[eit->second.vertex(i)] = eit->second.section_id();
        }
    }

    // record the fault DAS at the beginning of each section
    for (fit=_faults.begin(); fit!=_faults.end(); fit++) {
        sec_IDs = fit->second.section_ids();
        DAStotal = 0;

        for (sidit=sec_IDs.begin(); sidit!=sec_IDs.end(); sidit++) {
            sectStartDAS[*sidit] = DAStotal;
            DAStotal += sectLengths[*sidit];
        }
    }

    if (vertDASbysec == true){
		// Re-record each vertex DAS as it's currently known section DAS + DAS at beginning of section
		for (vit=_vertices.begin(); vit!=_vertices.end(); vit++) {
			vit->second.set_das(vit->second.das() + sectStartDAS[vertSects[vit->second.id()]]);
		}
	}

    // If tapering, loop through elements, calc their midpoint DAS,
    // assign horizontal sqrt tapering in end 12km.
    if (taper_method == "taper" || taper_method == "taper_full" || taper_method == "taper_renorm") {
        std::map<UIndex, float> fault_taper_full;
        std::map<UIndex, float> fault_taper_flow;

        for (fit=_faults.begin(); fit!=_faults.end(); fit++) {
            fault_taper_full[fit->first] = 0;
            fault_taper_flow[fit->first] = 0;
        }

        for (eit=_elements.begin(); eit!=_elements.end(); eit++) {
            simElem = create_sim_element(eit->second.id());
            eldas = simElem.min_das()+(simElem.max_das()-simElem.min_das())/2.0;
            sid = eit->second.section_id();
            fid = _sections[sid].fault_id();

            fault_taper_full[fid] += simElem.area() * eit->second.slip_rate();

            faultlength = _faults[fid].length();
            innerdist = faultlength/2.0 - fabs(faultlength/2.0 - eldas);

            if (innerdist < 12000) {
                taper_factor = sqrt(innerdist/12000.0);
                newSlipRate = eit->second.slip_rate() * taper_factor;
                eit->second.set_slip_rate(newSlipRate);
                fault_taper_flow[fid] += taper_factor*simElem.area()*eit->second.slip_rate();
            } else {
                fault_taper_flow[fid] += simElem.area()*eit->second.slip_rate();
            }
        }

        //If taper_renorm, loop back through elements and boost their slip rates by normalization factor
        if (taper_method == "taper_renorm") {
            for (eit=_elements.begin(); eit!=_elements.end(); eit++) {
                sid = eit->second.section_id();
                fid = _sections[sid].fault_id();
                renorm_factor = fault_taper_full[fid] / fault_taper_flow[fid];
                newSlipRate = eit->second.slip_rate() * renorm_factor;
                eit->second.set_slip_rate(newSlipRate);
            }
        }

    }


}

// Schultz: Adapted from Steve Ward's model, used in the EQSim comparison for UCERF2 model
void quakelib::ModelWorld::compute_stress_drops(const double &stress_drop_factor) {
    std::map<UIndex, ModelElement>::iterator eit;
    UIndex     fault_id;
    Conversion conv;
    ModelFault this_fault;
    double fault_area, fault_length, fault_width, char_magnitude, char_slip, R, nu, stress_drop;

    // Assign a stress drop to each element based on the geometry of the fault it belongs to
    for (eit=_elements.begin(); eit!=_elements.end(); eit++) {

        fault_id = section(eit->second.section_id()).fault_id();
        this_fault = fault(fault_id);

        fault_area = this_fault.area();
        fault_length = this_fault.length();
        fault_width = fault_area/fault_length;

        char_magnitude = 4.0+log10(conv.sqm2sqkm(fault_area)) + stress_drop_factor;
        char_slip = pow(10, (3.0/2.0)*(char_magnitude+6.0))/(eit->second.lame_mu()*fault_area);

        nu = 0.5*eit->second.lame_lambda()/(eit->second.lame_mu() + eit->second.lame_lambda());
        R  = sqrt(fault_width*fault_width + fault_length*fault_length);

        stress_drop = -2*eit->second.lame_mu()*char_slip*( (1-nu)*fault_length/fault_width + fault_width/fault_length )/( (1-nu)*M_PI*R );

        eit->second.set_stress_drop(stress_drop);

    }

}





quakelib::ModelFault &quakelib::ModelWorld::new_fault(const UIndex &fid) {
    _faults.insert(std::make_pair(fid, ModelFault()));
    _faults.find(fid)->second.set_id(fid);
    return _faults.find(fid)->second;
}

quakelib::ModelSection &quakelib::ModelWorld::new_section(void) {
    UIndex  max_ind = next_section_index();
    _sections.insert(std::make_pair(max_ind, ModelSection()));
    _sections.find(max_ind)->second.set_id(max_ind);
    return _sections.find(max_ind)->second;
}

quakelib::ModelElement &quakelib::ModelWorld::new_element(void) {
    UIndex  max_ind = next_element_index();
    _elements.insert(std::make_pair(max_ind, ModelElement()));
    _elements.find(max_ind)->second.set_id(max_ind);
    return _elements.find(max_ind)->second;
}



quakelib::ModelVertex &quakelib::ModelWorld::new_vertex(void) {
    UIndex  max_ind = next_vertex_index();
    _vertices.insert(std::make_pair(max_ind, ModelVertex()));
    _vertices.find(max_ind)->second.set_id(max_ind);
    return _vertices.find(max_ind)->second;
}

void quakelib::ModelVertex::get_field_descs(std::vector<FieldDesc> &descs) {
    FieldDesc       field_desc;

    field_desc.name = "id";
    field_desc.details = "Unique ID of the vertex.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(VertexData, _id);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "latitude";
    field_desc.details = "Latitude of the vertex.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(VertexData, _lat);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "longitude";
    field_desc.details = "Longitude of the vertex.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(VertexData, _lon);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "altitude";
    field_desc.details = "Altitude of the vertex in meters (negative is below ground).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(VertexData, _alt);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "das";
    field_desc.details = "Vertex distance along fault strike in meters.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(VertexData, _das);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "is_trace";
    field_desc.details = "Whether an element in on the fault trace (non-zero) or not (zero).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(VertexData, _is_trace);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

}

void quakelib::ModelVertex::read_data(const VertexData &in_data) {
    memcpy(&_data, &in_data, sizeof(VertexData));
}

void quakelib::ModelVertex::write_data(VertexData &out_data) const {
    memcpy(&out_data, &_data, sizeof(VertexData));
}

void quakelib::ModelVertex::read_ascii(std::istream &in_stream) {
    std::stringstream   ss(next_line(in_stream));

    ss >> _data._id;
    ss >> _data._lat;
    ss >> _data._lon;
    ss >> _data._alt;
    ss >> _data._das;
    ss >> _data._is_trace;
}

void quakelib::ModelVertex::write_ascii(std::ostream &out_stream) const {
    out_stream << _data._id << " ";
    out_stream << _data._lat << " ";
    out_stream << _data._lon << " ";
    out_stream << _data._alt << " ";
    out_stream << _data._das << " ";
    out_stream << _data._is_trace;
    next_line(out_stream);
}

void quakelib::ModelWorld::clear(void) {
    _faults.clear();
    _sections.clear();
    _elements.clear();
    _vertices.clear();
}

void quakelib::ModelWorld::clear_faults(void) {
    _faults.clear();
}

int quakelib::ModelWorld::read_file_ascii(const std::string &file_name) {
    std::ifstream       in_file;
    unsigned int        i, num_sections, num_elements, num_vertices, num_faults;
    double              stress_drop_factor;
    LatLonDepth         min_latlon, max_latlon;

    // Clear the world first to avoid incorrectly mixing indices
    clear();

    in_file.open(file_name.c_str());

    if (!in_file.is_open()) return -1;

    // Read the first line describing the number of sections, etc
    std::stringstream desc_line(next_line(in_file));
    desc_line >> num_faults;
    desc_line >> num_sections;
    desc_line >> num_elements;
    desc_line >> num_vertices;
    desc_line >> stress_drop_factor;

    setStressDropFactor(stress_drop_factor);

    // Read faults
    for (i=0; i<num_faults; ++i) {
        ModelFault     new_fault;
        new_fault.read_ascii(in_file);
        _faults.insert(std::make_pair(new_fault.id(), new_fault));
    }

    // Read sections
    for (i=0; i<num_sections; ++i) {
        ModelSection     new_section;
        new_section.read_ascii(in_file);
        _sections.insert(std::make_pair(new_section.id(), new_section));
    }

    // Read elements
    for (i=0; i<num_elements; ++i) {
        ModelElement     new_elem;
        new_elem.read_ascii(in_file);
        _elements.insert(std::make_pair(new_elem.id(), new_elem));
    }

    // Read vertices
    for (i=0; i<num_vertices; ++i) {
        ModelVertex     new_vert;
        new_vert.read_ascii(in_file);
        _vertices.insert(std::make_pair(new_vert.id(), new_vert));
    }

    in_file.close();

    // Reset the internal Cartesian coordinate system
    get_bounds(min_latlon, max_latlon);
    min_latlon.set_altitude(0);
    reset_base_coord(min_latlon);
    // Keep track of Lat/Lon bounds in the ModelWorld
    _min_lat = min_latlon.lat();
    _min_lon = min_latlon.lon();
    _max_lat = max_latlon.lat();
    _max_lon = max_latlon.lon();

    return 0;
}

int quakelib::ModelWorld::read_file_trace_latlon(std::vector<unsigned int> &unused_trace_segments, const std::string &file_name, const float &elem_size, const std::string &taper_method, const bool resize_trace_elements) {
    std::ifstream                   in_file;
    std::vector<FaultTracePoint>    trace_pts;
    std::string                     cur_section_name;
    unsigned int                    i, num_trace_pts;
    UIndex                          fault_id, sec_id;
    double                          min_lat, min_lon;

    // Clear the world first to avoid incorrectly mixing indices
    clear();

    in_file.open(file_name.c_str());

    if (!in_file.is_open()) return -1;

    while (in_file) {
        std::stringstream           ss(next_line(in_file));
        ModelWorld                  new_world;

        cur_section_name = "";
        num_trace_pts = 0;

        ss >> fault_id;
        ss >> sec_id;
        ss >> num_trace_pts;
        ss >> cur_section_name;

        if (cur_section_name.empty() || num_trace_pts == 0) break;

        min_lat = min_lon = DBL_MAX;

        for (i=0; i<num_trace_pts; ++i) {
            FaultTracePoint new_trace_pt(in_file);
            trace_pts.push_back(new_trace_pt);
            min_lat = fmin(min_lat, new_trace_pt.pos().lat());
            min_lon = fmin(min_lon, new_trace_pt.pos().lon());
        }

        new_world.create_section(unused_trace_segments, trace_pts, LatLonDepth(min_lat, min_lon), fault_id, elem_size, cur_section_name, taper_method, resize_trace_elements, sec_id);
        this->insert(new_world);
    }

    in_file.close();

    return 0;
}

void quakelib::ModelWorld::reset_base_coord(const LatLonDepth &new_base) {
    std::map<UIndex, ModelVertex>::iterator         it;

    for (it=_vertices.begin(); it!=_vertices.end(); ++it) {
        it->second.set_lld(it->second.lld(), new_base);
    }

    _base = new_base;
}

// Assumes elements will be in order along the trace
// TODO: add element comments to output
int quakelib::ModelWorld::write_file_trace_latlon(void) {
    eiterator           eit, last_element;
    siterator           sit;
    UIndex              sid;
    unsigned int        i;
    double              max_alt, min_alt, depth_along_dip, fault_depth, el_height;
    bool                element_on_trace;
    Conversion          c;
    std::string         sec_file_name;

    // Write traces by section
    for (sit=begin_section(); sit!=end_section(); ++sit) {
        std::vector<FaultTracePoint>    trace_pts;

        trace_pts.clear();
        sid = sit->id();

        std::ofstream out_file;
        sec_file_name = "trace_"+sit->name()+".txt";
        out_file.open(sec_file_name.c_str());

        // Start by going through all elements
        max_alt = -DBL_MAX;
        min_alt = DBL_MAX;

        for (eit=begin_element(sid); eit!=end_element(sid); ++eit) {
            for (i=0; i<3; ++i) {
                max_alt = fmax(max_alt, vertex(eit->vertex(i)).xyz()[2]);
                min_alt = fmin(min_alt, vertex(eit->vertex(i)).xyz()[2]);
            }
        }

        // Schultz: Here I am assuming a constant depth for the fault.
        // This should be improved later.
        fault_depth = fabs(max_alt-min_alt);

        // Go through the elements again and grab those which have vertices at the correct depth
        // TODO: interpolate between elements
        for (eit=begin_element(sid); eit!=end_element(sid); ++eit) {
            //element_on_trace = (vertex(eit->vertex(0)).xyz()[2] == max_alt);
            //Wilson: Sometimes trace points aren't at same height; here we use a quarter-element-height tolerance, or check the trace flag
            el_height = vertex(eit->vertex(0)).xyz()[2] - vertex(eit->vertex(1)).xyz()[2];
            element_on_trace = ( (vertex(eit->vertex(0)).xyz()[2] > max_alt-el_height/4.0) && (vertex(eit->vertex(0)).xyz()[2] < max_alt+el_height/4.0) ) || \
                               (vertex(eit->vertex(0)).is_trace() > 0);

            // If the element is on the trace, print it out
            if (element_on_trace) {
                Vec<3>      a, b;
                double      dip_angle;
                a = vertex(eit->vertex(1)).xyz() - vertex(eit->vertex(0)).xyz();
                b = vertex(eit->vertex(2)).xyz() - vertex(eit->vertex(0)).xyz();
                dip_angle = a.cross(b).unit_vector().vector_angle(Vec<3>(0,0,1));

                // Using the dip angle, compute the depth along dip
                if (dip_angle <= M_PI/2.0) {
                    depth_along_dip = fault_depth/sin(dip_angle);
                } else {
                    depth_along_dip = fault_depth/sin(M_PI - dip_angle);
                }

                FaultTracePoint trace_pt(vertex(eit->vertex(0)).lld(),
                                         depth_along_dip,
                                         c.m_per_sec2cm_per_yr(eit->slip_rate()),
                                         eit->aseismic(),
                                         c.rad2deg(eit->rake()),
                                         c.rad2deg(dip_angle),
                                         eit->lame_mu(),
                                         eit->lame_lambda());
                trace_pts.push_back(trace_pt);
                last_element = eit;
            }
        }

        Vec<3>      a, b;
        double      dip_angle;
        a = vertex(last_element->vertex(1)).xyz() - vertex(last_element->vertex(0)).xyz();
        b = vertex(last_element->vertex(2)).xyz() - vertex(last_element->vertex(0)).xyz();
        dip_angle = a.cross(b).unit_vector().vector_angle(Vec<3>(0,0,1));
        FaultTracePoint trace_pt(vertex(last_element->vertex(2)).lld(),
                                 depth_along_dip,
                                 c.m_per_sec2cm_per_yr(last_element->slip_rate()),
                                 last_element->aseismic(),
                                 c.rad2deg(last_element->rake()),
                                 c.rad2deg(dip_angle),
                                 last_element->lame_mu(),
                                 last_element->lame_lambda());
        trace_pts.push_back(trace_pt);

        // Write the fault header
        out_file << "# fault_id: ID number of the parent fault of this section\n";
        out_file << "# sec_id: ID number of this section\n";
        out_file << "# num_points: Number of trace points comprising this section\n";
        out_file << "# section_name: Name of the section\n";

        // Write out the recorded trace for this fault
        out_file << sit->fault_id() << " " << sit->id() << " " << trace_pts.size() << " " << sit->name() << "\n";

        // Write out the trace point header
        out_file << "# latitude: Latitude of trace point\n";
        out_file << "# longitude: Longitude of trace point\n";
        out_file << "# altitude: Altitude of trace point (meters)\n";
        out_file << "# depth_along_dip: Depth along dip (meters)\n";
        out_file << "# slip_rate: Slip rate at trace point (centimeters/year)\n";
        out_file << "# aseismic: Fraction of slip that is aseismic at point\n";
        out_file << "# rake: Fault rake at trace point (degrees)\n";
        out_file << "# dip: Fault dip at trace point (degrees)\n";
        out_file << "# lame_mu: Lame's mu parameter at trace point (Pascals)\n";
        out_file << "# lame_lambda: Lame's lambda parameter at trace point (Pascals)\n";

        // And each of the trace points
        for (i=0; i<trace_pts.size(); ++i) trace_pts[i].write_ascii(out_file);

        // Close the file
        out_file.close();
        std::cout << "Wrote trace file: " << sec_file_name << std::endl;
    }

    return 0;
}

// Write a single trace file for each fault; useful for remeshing long faults with many small segments (eg UCERF3)
//TODO: Keep track of fault names, add those names to the file name and header
int quakelib::ModelWorld::write_file_trace_latlon_faultwise(void) {
    eiterator           eit, last_element;
    siterator           sit;
    fiterator           fit;
    UIndex              fid;
    UIndex              sid;
    unsigned int        i;
    double              max_alt, min_alt, depth_along_dip, section_depth, el_height;
    bool                element_on_trace;
    Conversion          c;
    std::string         fault_file_name;
    std::stringstream   ss;

    // Write traces by fault
    for (fit=begin_fault(); fit!=end_fault(); ++fit) {
        std::vector<FaultTracePoint>    trace_pts;

        trace_pts.clear();
        fid = fit->id();

        std::ofstream out_file;
        fault_file_name = "trace_"+fit->name()+".txt";
        out_file.open(fault_file_name.c_str());


        for (sit=begin_section(); sit!=end_section(); ++sit) {

            sid = sit->id();

            if (sit->fault_id()==fid) {

                // Start by going through all elements
                max_alt = -DBL_MAX;
                min_alt = DBL_MAX;

                for (eit=begin_element(sid); eit!=end_element(sid); ++eit) {
                    for (i=0; i<3; ++i) {
                        max_alt = fmax(max_alt, vertex(eit->vertex(i)).xyz()[2]);
                        min_alt = fmin(min_alt, vertex(eit->vertex(i)).xyz()[2]);
                    }
                }

                // Schultz: Here I am assuming a constant depth for the section.
                // This should be improved later.
                section_depth = fabs(max_alt-min_alt);

                // Go through the elements again and grab those which have vertices at the correct depth
                // TODO: interpolate between elements
                for (eit=begin_element(sid); eit!=end_element(sid); ++eit) {
                    //Wilson: Sometimes trace points aren't at same height; here we use a quarter-element-height tolerance, or check the trace flag
                    el_height = vertex(eit->vertex(0)).xyz()[2] - vertex(eit->vertex(1)).xyz()[2];
                    element_on_trace = ( (vertex(eit->vertex(0)).xyz()[2] > max_alt-el_height/4.0) && (vertex(eit->vertex(0)).xyz()[2] < max_alt+el_height/4.0) ) || \
                                       (vertex(eit->vertex(0)).is_trace() > 0);

                    // If the element is on the trace, print it out
                    if (element_on_trace) {
                        Vec<3>      a, b;
                        double      dip_angle;
                        a = vertex(eit->vertex(1)).xyz() - vertex(eit->vertex(0)).xyz();
                        b = vertex(eit->vertex(2)).xyz() - vertex(eit->vertex(0)).xyz();
                        dip_angle = a.cross(b).unit_vector().vector_angle(Vec<3>(0,0,1));

                        // Using the dip angle, compute the depth along dip
                        if (dip_angle <= M_PI/2.0) {
                            depth_along_dip = section_depth/sin(dip_angle);
                        } else {
                            depth_along_dip = section_depth/sin(M_PI - dip_angle);
                        }

                        FaultTracePoint trace_pt(vertex(eit->vertex(0)).lld(),
                                                 depth_along_dip,
                                                 c.m_per_sec2cm_per_yr(eit->slip_rate()),
                                                 eit->aseismic(),
                                                 c.rad2deg(eit->rake()),
                                                 c.rad2deg(dip_angle),
                                                 eit->lame_mu(),
                                                 eit->lame_lambda());
                        trace_pts.push_back(trace_pt);
                        last_element = eit;
                    }
                }

                Vec<3>      a, b;
                double      dip_angle;
                a = vertex(last_element->vertex(1)).xyz() - vertex(last_element->vertex(0)).xyz();
                b = vertex(last_element->vertex(2)).xyz() - vertex(last_element->vertex(0)).xyz();
                dip_angle = a.cross(b).unit_vector().vector_angle(Vec<3>(0,0,1));
                FaultTracePoint trace_pt(vertex(last_element->vertex(2)).lld(),
                                         depth_along_dip,
                                         c.m_per_sec2cm_per_yr(last_element->slip_rate()),
                                         last_element->aseismic(),
                                         c.rad2deg(last_element->rake()),
                                         c.rad2deg(dip_angle),
                                         last_element->lame_mu(),
                                         last_element->lame_lambda());
                trace_pts.push_back(trace_pt);

            }

        }

        // Write the fault header
        out_file << "# fault_id: ID number of the parent fault of this section\n";
        out_file << "# sec_id: ID number of this section\n";
        out_file << "# num_points: Number of trace points comprising this section\n";
        out_file << "# section_name: Name of the section\n";

        // Write out the recorded trace for this fault
        out_file << fid << " " << fid << " " << trace_pts.size() << " " << fit->name() << "\n";

        // Write out the trace point header
        out_file << "# latitude: Latitude of trace point\n";
        out_file << "# longitude: Longitude of trace point\n";
        out_file << "# altitude: Altitude of trace point (meters)\n";
        out_file << "# depth_along_dip: Depth along dip (meters)\n";
        out_file << "# slip_rate: Slip rate at trace point (centimeters/year)\n";
        out_file << "# aseismic: Fraction of slip that is aseismic at point\n";
        out_file << "# rake: Fault rake at trace point (degrees)\n";
        out_file << "# dip: Fault dip at trace point (degrees)\n";
        out_file << "# lame_mu: Lame's mu parameter at trace point (Pascals)\n";
        out_file << "# lame_lambda: Lame's lambda parameter at trace point (Pascals)\n";

        // And each of the trace points
        for (i=0; i<trace_pts.size(); ++i) trace_pts[i].write_ascii(out_file);

        // Close the file
        out_file.close();
        std::cout << "Wrote trace file: " << fault_file_name << std::endl;
    }

    return 0;
}

int quakelib::ModelWorld::write_file_ascii(const std::string &file_name) const {
    std::ofstream                                   out_file;
    std::vector<FieldDesc>                          descs;
    std::vector<FieldDesc>::iterator                dit;
    std::map<UIndex, ModelVertex>::const_iterator   vit;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    std::map<UIndex, ModelSection>::const_iterator  fit;
    std::map<UIndex, ModelFault>::const_iterator    f_it;

    out_file.open(file_name.c_str());
    out_file << "# Number of faults\n";
    out_file << "# Number of sections\n";
    out_file << "# Number of elements\n";
    out_file << "# Number of vertices\n";
    out_file << "# Stress drop factor\n";
    out_file << _faults.size() << " " << _sections.size() << " " << _elements.size() << " " << _vertices.size() << " " << stressDropFactor() ;
    next_line(out_file);

    // Write fault header
    descs.clear();
    ModelFault::get_field_descs(descs);

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_file << "# " << dit->name << ": " << dit->details << "\n";
    }

    out_file << "# ";

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_file << dit->name << " ";
    }

    out_file << "\n";

    // Write faults
    for (f_it=_faults.begin(); f_it!=_faults.end(); ++f_it) {
        f_it->second.write_ascii(out_file);
    }


    // Write section header
    descs.clear();
    ModelSection::get_field_descs(descs);

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_file << "# " << dit->name << ": " << dit->details << "\n";
    }

    out_file << "# ";

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_file << dit->name << " ";
    }

    out_file << "\n";

    // Write sections
    for (fit=_sections.begin(); fit!=_sections.end(); ++fit) {
        fit->second.write_ascii(out_file);
    }

    // Write element header
    descs.clear();
    ModelElement::get_field_descs(descs);

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_file << "# " << dit->name << ": " << dit->details << "\n";
    }

    out_file << "# ";

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_file << dit->name << " ";
    }

    out_file << "\n";

    // Write elements
    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        eit->second.write_ascii(out_file);
    }

    // Write vertex header
    descs.clear();
    ModelVertex::get_field_descs(descs);

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_file << "# " << dit->name << ": " << dit->details << "\n";
    }

    out_file << "# ";

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_file << dit->name << " ";
    }

    out_file << "\n";

    // Write vertices
    for (vit=_vertices.begin(); vit!=_vertices.end(); ++vit) {
        vit->second.write_ascii(out_file);
    }

    out_file.close();

    return 0;
}

int quakelib::ModelWorld::read_file_hdf5(const std::string &file_name) {
#ifdef HDF5_FOUND
    hid_t       plist_id, data_file;
    herr_t      res;
    LatLonDepth min_latlon, max_latlon;

    // Clear the world first to avoid incorrectly mixing indices
    clear();

    if (!H5Fis_hdf5(file_name.c_str())) return -1;

    plist_id = H5Pcreate(H5P_FILE_ACCESS);

    if (plist_id < 0) exit(-1);

    data_file = H5Fopen(file_name.c_str(), H5F_ACC_RDONLY, plist_id);

    if (data_file < 0) exit(-1);

    read_stress_drop_factor_hdf5(data_file);
    read_fault_hdf5(data_file);
    read_section_hdf5(data_file);
    read_element_hdf5(data_file);
    read_vertex_hdf5(data_file);

    // Release HDF5 handles
    res = H5Pclose(plist_id);

    if (res < 0) exit(-1);

    res = H5Fclose(data_file);

    if (res < 0) exit(-1);

    // Reset the internal Cartesian coordinate system
    get_bounds(min_latlon, max_latlon);
    min_latlon.set_altitude(0);
    reset_base_coord(min_latlon);
    // Keep track of Lat/Lon bounds in the ModelWorld
    _min_lat = min_latlon.lat();
    _min_lon = min_latlon.lon();
    _max_lat = max_latlon.lat();
    _max_lon = max_latlon.lon();
#else
    // TODO: Error out
#endif
    return 0;
}

#ifdef HDF5_FOUND
void quakelib::ModelWorld::read_fault_hdf5(const hid_t &data_file) {
    std::vector<FieldDesc>                          descs;
    std::map<UIndex, ModelFault>::const_iterator  fit;
    hsize_t                     num_fields, num_faults;
    unsigned int                i;
    FaultData                 *fault_data;
    size_t                      *field_offsets;
    size_t                      *field_sizes;
    herr_t                      res;

    descs.clear();
    ModelFault::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    res = H5TBget_table_info(data_file, ModelFault::hdf5_table_name().c_str(), &num_fields, &num_faults);

    if (res < 0) exit(-1);

    // TODO: check that num_fields matches the descs
    //
    fault_data = new FaultData[num_faults];
    res = H5TBread_records(data_file, ModelFault::hdf5_table_name().c_str(), 0, num_faults, sizeof(FaultData), field_offsets, field_sizes, fault_data);

    if (res < 0) exit(-1);

    // Read section data into the World
    for (i=0; i<num_faults; ++i) {
        ModelFault  new_fault;
        new_fault.read_data(fault_data[i]);
        _faults.insert(std::make_pair(new_fault.id(), new_fault));
    }

    // Free memory for HDF5 related data
    // yoder: ... and use delete [] for arrays...
    delete [] fault_data;
    delete [] field_offsets;
    delete [] field_sizes;

}


void quakelib::ModelWorld::read_section_hdf5(const hid_t &data_file) {
    std::vector<FieldDesc>                          descs;
    std::map<UIndex, ModelSection>::const_iterator  fit;
    hsize_t                     num_fields, num_sections;
    unsigned int                i;
    SectionData                 *section_data;
    size_t                      *field_offsets;
    size_t                      *field_sizes;
    herr_t                      res;

    descs.clear();
    ModelSection::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    res = H5TBget_table_info(data_file, ModelSection::hdf5_table_name().c_str(), &num_fields, &num_sections);

    if (res < 0) exit(-1);

    // TODO: check that num_fields matches the descs
    //
    section_data = new SectionData[num_sections];
    res = H5TBread_records(data_file, ModelSection::hdf5_table_name().c_str(), 0, num_sections, sizeof(SectionData), field_offsets, field_sizes, section_data);

    if (res < 0) exit(-1);

    // Read section data into the World
    for (i=0; i<num_sections; ++i) {
        ModelSection  new_section;
        new_section.read_data(section_data[i]);
        _sections.insert(std::make_pair(new_section.id(), new_section));
    }

    // Free memory for HDF5 related data
    // yoder: ... and use delete [] for arrays...
    delete [] section_data;
    delete [] field_offsets;
    delete [] field_sizes;

}

void quakelib::ModelWorld::read_element_hdf5(const hid_t &data_file) {
    std::vector<FieldDesc>                          descs;
    std::map<UIndex, ModelElement>::const_iterator  fit;
    hsize_t                     num_fields, num_elements;
    unsigned int                i;
    ElementData                 *element_data;
    size_t                      *field_offsets;
    size_t                      *field_sizes;
    herr_t                      res;

    descs.clear();
    ModelElement::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    res = H5TBget_table_info(data_file, ModelElement::hdf5_table_name().c_str(), &num_fields, &num_elements);

    if (res < 0) exit(-1);

    // TODO: check that num_fields matches the descs

    element_data = new ElementData[num_elements];
    res = H5TBread_records(data_file, ModelElement::hdf5_table_name().c_str(), 0, num_elements, sizeof(ElementData), field_offsets, field_sizes, element_data);

    if (res < 0) exit(-1);

    // Read element data into the World
    for (i=0; i<num_elements; ++i) {
        ModelElement  new_element;
        new_element.read_data(element_data[i]);
        _elements.insert(std::make_pair(new_element.id(), new_element));
    }

    // Free memory for HDF5 related data
    delete [] element_data;
    delete [] field_offsets;
    delete [] field_sizes;
}

void quakelib::ModelWorld::read_vertex_hdf5(const hid_t &data_file) {
    std::vector<FieldDesc>                          descs;
    std::map<UIndex, ModelVertex>::const_iterator  fit;
    hsize_t                     num_fields, num_vertices;
    unsigned int                i;
    VertexData                  *vertex_data;
    size_t                      *field_offsets;
    size_t                      *field_sizes;
    herr_t                      res;

    descs.clear();
    ModelVertex::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    res = H5TBget_table_info(data_file, ModelVertex::hdf5_table_name().c_str(), &num_fields, &num_vertices);

    if (res < 0) exit(-1);

    // TODO: check that num_fields matches the descs

    vertex_data = new VertexData[num_vertices];
    res = H5TBread_records(data_file, ModelVertex::hdf5_table_name().c_str(), 0, num_vertices, sizeof(VertexData), field_offsets, field_sizes, vertex_data);

    if (res < 0) exit(-1);

    // Read vertex data into the World
    for (i=0; i<num_vertices; ++i) {
        ModelVertex  new_vertex;
        new_vertex.read_data(vertex_data[i]);
        _vertices.insert(std::make_pair(new_vertex.id(), new_vertex));
    }

    // Free memory for HDF5 related data
    // yoder: ... and use delete [] for vector/array types...
    delete [] vertex_data;
    delete [] field_offsets;
    delete [] field_sizes;
}

void quakelib::ModelWorld::write_stress_drop_factor_hdf5(const hid_t &data_file) const {
    double  tmp[2];
    hid_t   values_set;
    hid_t   pair_val_dataspace;
    hsize_t dimsf[2];
    herr_t  status, res;

    // Create dataspace for a single value
    dimsf[0] = 1;
    pair_val_dataspace = H5Screate_simple(1, dimsf, NULL);

    // Create entries for the simulation start/stop years and base longitude/latitude
    values_set = H5Dcreate2(data_file, "stress_drop_factor", H5T_NATIVE_DOUBLE, pair_val_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    if (values_set < 0) exit(-1);

    // Record the simulation start/end years
    tmp[0] = stressDropFactor();
    tmp[1] = stressDropFactor();
    status = H5Dwrite(values_set, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &tmp);

    // Close the handles we've used
    res = H5Dclose(values_set);
}

void quakelib::ModelWorld::read_stress_drop_factor_hdf5(const hid_t &data_file) {
    double  tmp;
    herr_t  res;
    hid_t   data_id, data_access_properties;

    // Get data set property list
    data_access_properties = H5Pcreate(H5P_DATASET_ACCESS);

    // Open the data set for reading
    data_id = H5Dopen2(data_file, "stress_drop_factor", data_access_properties);

    if (data_id < 0) exit(-1);

    // Read the stress drop data
    res = H5Dread(data_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &tmp);

    if (res < 0) exit(-1);

    // Set the stress drop factor for the model
    setStressDropFactor(tmp);

}

void quakelib::ModelWorld::write_fault_hdf5(const hid_t &data_file) const {
    std::vector<FieldDesc>                          descs;
    std::map<UIndex, ModelFault>::const_iterator  fit;
    size_t                      num_fields, num_faults;
    unsigned int                i;
    FaultData                   blank_fault, *fault_data;
    char                        **field_names, **field_details;
    size_t                      *field_offsets;
    hid_t                       *field_types;
    size_t                      *field_sizes;
    herr_t                      res;

    // Set up the section table definition
    descs.clear();
    ModelFault::get_field_descs(descs);
    num_fields = descs.size();
    num_faults = _faults.size();
    field_names = new char *[num_fields];
    field_details = new char *[num_fields];
    field_offsets = new size_t[num_fields];
    field_types = new hid_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_names[i] = new char[descs[i].name.length()+1];
        strncpy(field_names[i], descs[i].name.c_str(), descs[i].name.length());
        field_names[i][descs[i].name.length()] = '\0';
        field_details[i] = new char[descs[i].details.length()+1];
        strncpy(field_details[i], descs[i].details.c_str(), descs[i].details.length());
        field_details[i][descs[i].details.length()] = '\0';
        field_offsets[i] = descs[i].offset;
        field_types[i] = descs[i].type;
        field_sizes[i] = descs[i].size;
    }

    // TODO: factor this out?
    blank_fault = ModelFault().data();

    // Fill in the data for the sections
    fault_data = new FaultData[num_faults];

    for (i=0,fit=_faults.begin(); fit!=_faults.end(); ++i,++fit) {
        fit->second.write_data(fault_data[i]);
    }

    // Create the section table
    res = H5TBmake_table("Faults",
                         data_file,
                         ModelFault::hdf5_table_name().c_str(),
                         num_fields,
                         num_faults,
                         sizeof(FaultData),
                         (const char **)field_names,
                         field_offsets,
                         field_types,
                         num_faults,
                         &blank_fault,
                         0,
                         fault_data);

    if (res < 0) exit(-1);

    // Add the details of each field as an attribute
    for (i=0; i<num_fields; ++i) {
        std::stringstream   ss;
        ss << "FIELD_" << i << "_DETAILS";
        res = H5LTset_attribute_string(data_file,
                                       ModelFault::hdf5_table_name().c_str(),
                                       ss.str().c_str(),
                                       field_details[i]);

        if (res < 0) exit(-1);
    }

    // Free memory for HDF5 related data
    delete fault_data;

    for (i=0; i<num_fields; ++i) delete field_names[i];

    delete field_names;

    for (i=0; i<num_fields; ++i) delete field_details[i];

    delete field_details;
    delete field_offsets;
    delete field_types;
    delete field_sizes;
}

void quakelib::ModelWorld::write_section_hdf5(const hid_t &data_file) const {
    std::vector<FieldDesc>                          descs;
    std::map<UIndex, ModelSection>::const_iterator  fit;
    size_t                      num_fields, num_sections;
    unsigned int                i;
    SectionData                 blank_section, *section_data;
    char                        **field_names, **field_details;
    size_t                      *field_offsets;
    hid_t                       *field_types;
    size_t                      *field_sizes;
    herr_t                      res;

    // Set up the section table definition
    descs.clear();
    ModelSection::get_field_descs(descs);
    num_fields = descs.size();
    num_sections = _sections.size();
    field_names = new char *[num_fields];
    field_details = new char *[num_fields];
    field_offsets = new size_t[num_fields];
    field_types = new hid_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_names[i] = new char[descs[i].name.length()+1];
        strncpy(field_names[i], descs[i].name.c_str(), descs[i].name.length());
        field_names[i][descs[i].name.length()] = '\0';
        field_details[i] = new char[descs[i].details.length()+1];
        strncpy(field_details[i], descs[i].details.c_str(), descs[i].details.length());
        field_details[i][descs[i].details.length()] = '\0';
        field_offsets[i] = descs[i].offset;
        field_types[i] = descs[i].type;
        field_sizes[i] = descs[i].size;
    }

    // TODO: factor this out?
    blank_section = ModelSection().data();

    // Fill in the data for the sections
    section_data = new SectionData[num_sections];

    for (i=0,fit=_sections.begin(); fit!=_sections.end(); ++i,++fit) {
        fit->second.write_data(section_data[i]);
    }

    // Create the section table
    res = H5TBmake_table("Fault Sections",
                         data_file,
                         ModelSection::hdf5_table_name().c_str(),
                         num_fields,
                         num_sections,
                         sizeof(SectionData),
                         (const char **)field_names,
                         field_offsets,
                         field_types,
                         num_sections,
                         &blank_section,
                         0,
                         section_data);

    if (res < 0) exit(-1);

    // Add the details of each field as an attribute
    for (i=0; i<num_fields; ++i) {
        std::stringstream   ss;
        ss << "FIELD_" << i << "_DETAILS";
        res = H5LTset_attribute_string(data_file,
                                       ModelSection::hdf5_table_name().c_str(),
                                       ss.str().c_str(),
                                       field_details[i]);

        if (res < 0) exit(-1);
    }

    // Free memory for HDF5 related data
    delete section_data;

    for (i=0; i<num_fields; ++i) delete field_names[i];

    delete field_names;

    for (i=0; i<num_fields; ++i) delete field_details[i];

    delete field_details;
    delete field_offsets;
    delete field_types;
    delete field_sizes;
}

void quakelib::ModelWorld::write_element_hdf5(const hid_t &data_file) const {
    std::vector<FieldDesc>                          descs;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    size_t                      num_fields, num_elements;
    unsigned int                i;
    ElementData                 blank_element, *element_data;
    char                        **field_names, **field_details;
    size_t                      *field_offsets;
    hid_t                       *field_types;
    size_t                      *field_sizes;
    herr_t                      res;

    // Set up the element table definition
    descs.clear();
    ModelElement::get_field_descs(descs);
    num_fields = descs.size();
    num_elements = _elements.size();
    field_names = new char *[num_fields];
    field_details = new char *[num_fields];
    field_offsets = new size_t[num_fields];
    field_types = new hid_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_names[i] = new char[descs[i].name.length()+1];
        strncpy(field_names[i], descs[i].name.c_str(), descs[i].name.length());
        field_names[i][descs[i].name.length()] = '\0';
        field_details[i] = new char[descs[i].details.length()+1];
        strncpy(field_details[i], descs[i].details.c_str(), descs[i].details.length());
        field_details[i][descs[i].details.length()] = '\0';
        field_offsets[i] = descs[i].offset;
        field_types[i] = descs[i].type;
        field_sizes[i] = descs[i].size;
    }

    // Get a blank element for table filling
    blank_element = ModelElement().data();

    // Fill in the data for the elements
    element_data = new ElementData[num_elements];

    for (i=0,eit=_elements.begin(); eit!=_elements.end(); ++i,++eit) {
        eit->second.write_data(element_data[i]);
    }

    // Create the elements table
    res = H5TBmake_table("Elements",
                         data_file,
                         ModelElement::hdf5_table_name().c_str(),
                         num_fields,
                         num_elements,
                         sizeof(ElementData),
                         (const char **)field_names,
                         field_offsets,
                         field_types,
                         num_elements,
                         &blank_element,
                         0,
                         element_data);

    if (res < 0) exit(-1);

    // Add the details of each field as an attribute
    for (i=0; i<num_fields; ++i) {
        std::stringstream   ss;
        ss << "FIELD_" << i << "_DETAILS";
        res = H5LTset_attribute_string(data_file,
                                       ModelElement::hdf5_table_name().c_str(),
                                       ss.str().c_str(),
                                       field_details[i]);

        if (res < 0) exit(-1);
    }

    // Free memory for HDF5 related data
    delete element_data;

    for (i=0; i<num_fields; ++i) delete field_names[i];

    delete field_names;

    for (i=0; i<num_fields; ++i) delete field_details[i];

    delete field_details;
    delete field_offsets;
    delete field_types;
    delete field_sizes;
}

void quakelib::ModelWorld::write_vertex_hdf5(const hid_t &data_file) const {
    std::vector<FieldDesc>                          descs;
    std::map<UIndex, ModelVertex>::const_iterator   vit;
    size_t                      num_fields, num_vertices;
    unsigned int                i;
    VertexData                  blank_vertex, *vertex_data;
    char                        **field_names, **field_details;
    size_t                      *field_offsets;
    hid_t                       *field_types;
    size_t                      *field_sizes;
    herr_t                      res;

    // Set up the vertex table definition
    descs.clear();
    ModelVertex::get_field_descs(descs);
    num_fields = descs.size();
    num_vertices = _vertices.size();
    field_names = new char *[num_fields];
    field_details = new char *[num_fields];
    field_offsets = new size_t[num_fields];
    field_types = new hid_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_names[i] = new char[descs[i].name.length()+1];
        strncpy(field_names[i], descs[i].name.c_str(), descs[i].name.length());
        field_names[i][descs[i].name.length()] = '\0';
        field_details[i] = new char[descs[i].details.length()+1];
        strncpy(field_details[i], descs[i].details.c_str(), descs[i].details.length());
        field_details[i][descs[i].details.length()] = '\0';
        field_offsets[i] = descs[i].offset;
        field_types[i] = descs[i].type;
        field_sizes[i] = descs[i].size;
    }

    blank_vertex = ModelVertex().data();

    // Fill in the data for the vertices
    vertex_data = new VertexData[num_vertices];

    for (i=0,vit=_vertices.begin(); vit!=_vertices.end(); ++i,++vit) {
        vit->second.write_data(vertex_data[i]);
    }

    // Create the vertices table
    res = H5TBmake_table("Vertices",
                         data_file,
                         ModelVertex::hdf5_table_name().c_str(),
                         num_fields,
                         num_vertices,
                         sizeof(VertexData),
                         (const char **)field_names,
                         field_offsets,
                         field_types,
                         num_vertices,
                         &blank_vertex,
                         0,
                         vertex_data);

    if (res < 0) exit(-1);

    // Add the details of each field as an attribute
    for (i=0; i<num_fields; ++i) {
        std::stringstream   ss;
        ss << "FIELD_" << i << "_DETAILS";
        res = H5LTset_attribute_string(data_file,
                                       ModelVertex::hdf5_table_name().c_str(),
                                       ss.str().c_str(),
                                       field_details[i]);

        if (res < 0) exit(-1);
    }

    // Free memory for HDF5 related data
    delete [] vertex_data;

    for (i=0; i<num_fields; ++i) delete [] field_names[i];

    delete [] field_names;

    for (i=0; i<num_fields; ++i) delete [] field_details[i];

    delete [] field_details;
    delete [] field_offsets;
    delete [] field_types;
    delete [] field_sizes;
}
#endif

int quakelib::ModelWorld::write_file_hdf5(const std::string &file_name) const {
#ifdef HDF5_FOUND
    hid_t       plist_id, data_file;
    herr_t      res;

    // Create access properties
    plist_id = H5Pcreate(H5P_FILE_ACCESS);

    if (plist_id < 0) exit(-1);

    // Create the data file, overwriting any old files
    data_file = H5Fcreate(file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);

    if (data_file < 0) exit(-1);

    write_stress_drop_factor_hdf5(data_file);
    write_fault_hdf5(data_file);
    write_section_hdf5(data_file);
    write_element_hdf5(data_file);
    write_vertex_hdf5(data_file);

    // Release HDF5 handles
    res = H5Pclose(plist_id);

    if (res < 0) exit(-1);

    res = H5Fclose(data_file);

    if (res < 0) exit(-1);

    return 0;
#else
    return 1;
#endif
}

void quakelib::ModelWorld::get_bounds(LatLonDepth &minimum, LatLonDepth &maximum) const {
    std::map<UIndex, ModelVertex>::const_iterator    it;
    double      min_lat, min_lon, min_alt;
    double      max_lat, max_lon, max_alt;

    min_lat = min_lon = min_alt = DBL_MAX;
    max_lat = max_lon = max_alt = -DBL_MAX;

    for (it=_vertices.begin(); it!=_vertices.end(); ++it) {
        min_lat = fmin(min_lat, it->second.lld().lat());
        max_lat = fmax(max_lat, it->second.lld().lat());
        min_lon = fmin(min_lon, it->second.lld().lon());
        max_lon = fmax(max_lon, it->second.lld().lon());
        min_alt = fmin(min_alt, it->second.lld().altitude());
        max_alt = fmax(max_alt, it->second.lld().altitude());
    }

    if (min_lat == DBL_MAX || min_lon == DBL_MAX || min_alt == DBL_MAX) {
        minimum = LatLonDepth();
    } else {
        minimum = LatLonDepth(min_lat, min_lon, min_alt);
    }

    if (max_lat == -DBL_MAX || max_lon == -DBL_MAX || max_alt == -DBL_MAX) {
        maximum = LatLonDepth();
    } else {
        maximum = LatLonDepth(max_lat, max_lon, max_alt);
    }
}

quakelib::SimElement quakelib::ModelWorld::create_sim_element(const UIndex &element_id) const {
    SimElement          new_element;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    std::map<UIndex, ModelVertex>::const_iterator   vit;
    unsigned int        i;

    eit = _elements.find(element_id);

    for (i=0; i<3; ++i) {
        vit = _vertices.find(eit->second.vertex(i));
        new_element.set_vert(i, vit->second.xyz());
        new_element.set_das(i, vit->second.das());
    }

    new_element.set_is_quad(eit->second.is_quad());
    new_element.set_rake(eit->second.rake());
    new_element.set_slip_rate(eit->second.slip_rate());
    new_element.set_aseismic(eit->second.aseismic());
    new_element.set_lame_mu(eit->second.lame_mu());
    new_element.set_lame_lambda(eit->second.lame_lambda());
    new_element.set_max_slip(eit->second.max_slip());
    new_element.set_stress_drop(eit->second.stress_drop());

    return new_element;
}

quakelib::SlippedElement quakelib::ModelWorld::create_slipped_element(const UIndex &element_id) const {
    SlippedElement          new_element;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    std::map<UIndex, ModelVertex>::const_iterator   vit;
    unsigned int        i;

    eit = _elements.find(element_id);

    for (i=0; i<3; ++i) {
        vit = _vertices.find(eit->second.vertex(i));
        new_element.set_vert(i, vit->second.xyz());
    }

    // TODO, vertices lat/lon or xyz, save base?
    new_element.set_id(eit->second.id());
    new_element.set_section_id(eit->second.section_id());
    new_element.set_is_quad(eit->second.is_quad());
    new_element.set_rake(eit->second.rake());
    new_element.set_slip_rate(eit->second.slip_rate());
    new_element.set_aseismic(eit->second.aseismic());
    new_element.set_lame_mu(eit->second.lame_mu());
    new_element.set_lame_lambda(eit->second.lame_lambda());
    new_element.set_max_slip(eit->second.max_slip());
    new_element.set_slip(0.0);

    return new_element;
}

int quakelib::ModelWorld::read_files_eqsim(const std::string &geom_file_name, const std::string &cond_file_name, const std::string &fric_file_name, const std::string &taper_method) {
    quakelib::ModelWorld            eqsim_world;
    quakelib::EQSimGeometryReader   geometry_data;
    quakelib::EQSimConditionReader  condition_data;
    quakelib::EQSimFrictionReader   friction_data;
    quakelib::EQSimErrors           error_report;
    bool                            read_cond_file;
    quakelib::EQSimGeomSectionMap::const_iterator   sit;
    quakelib::EQSimGeomRectangleMap::const_iterator it;
    quakelib::eiterator             eit, eeit;
    quakelib::siterator             sec_it;
    quakelib::LatLonDepth           base;
    std::map<UIndex, double>        fault_areas;
    double                          taper_t;
    std::map<UIndex, double> section_taper_full;
    std::map<UIndex, double> section_taper_flow;
    // Schultz: The structure below is a map from each fault_id to another map, which is (at this fault)
    //    a map from the distinct values of DAS (distance along strike) to a set of element IDs.
    //    Use this to access the elements above and below any particular element.
    std::map<UIndex, std::map<double, ElementIDSet> >  faults_with_elements_at_each_das;
    ElementIDSet::const_iterator    id_it;

    // Clear the world first to avoid incorrectly mixing indices
    clear();

    geometry_data.parse_file(geom_file_name);
    geometry_data.validate(error_report);
    error_report.write(std::cerr);
    error_report.clear();

    friction_data.parse_file(fric_file_name);
    friction_data.validate(error_report);
    error_report.write(std::cerr);
    error_report.clear();

    read_cond_file = condition_data.parse_file(cond_file_name);

    if (read_cond_file) {
        condition_data.validate(error_report);
        error_report.write(std::cerr);
        error_report.clear();
    }

    // Take the conversion base as the middle of the section map
    base = quakelib::LatLonDepth(geometry_data.lat_lo(), geometry_data.lon_lo());

    // Initiate converter
    Conversion          conv(base);

    // Triangle elements are currently not supported
    if (geometry_data.num_triangles() > 0) {
        std::cerr << "ERROR: Currently cannot handle EQSim triangle elements. These elements will be ignored." << std::endl;
    }

    // Go through the geometry and create sections for each EQSim section
    for (sit=geometry_data.sections.begin(); sit!=geometry_data.sections.end(); ++sit) {
        eqsim_world.insert(sit->second.create_model_section());

        // Assuming aligned rectangular elements
        for (it=sit->second.rectangles.begin(); it!=sit->second.rectangles.end(); ++it) {
            quakelib::ModelElement   new_element;
            unsigned int             i;

            // Initialize fault slip rate tapering data structures
            section_taper_flow[sit->second.sid()] = 0.0;
            section_taper_full[sit->second.sid()] = 0.0;

            // Kasey: Note, creating the elements one by one while reading each line enforces
            // that the element id's match the index in the EQSim files. So that new_element.id()
            // corresponds to the index (or EQSim element number) which is it->second.index()
            new_element = it->second.create_model_element();
            new_element.set_section_id(sit->second.sid());

            for (i=0; i<4; ++i) eqsim_world.insert(sit->second.vertices.find(it->second.vertex(i))->second.create_model_vertex(base));

            new_element.set_lame_lambda(friction_data.get_lame_lambda());
            new_element.set_lame_mu(friction_data.get_lame_mu());
            new_element.set_max_slip(0);    // Set a temporary maximum slip of 0 (this will be changed below)

            // Static strengths are saved as positive values, stress_drop = -static_strength
            // May need to include stress_drop = -(static_strength-dynamic_strength) in future
            new_element.set_stress_drop(-friction_data.get_static_strength(new_element.id()));

            // Insert partially finished element into the eqsim_world
            eqsim_world.insert(new_element);

            // Compute area of the current element, add it to the total for this section
            quakelib::SimElement this_element = eqsim_world.create_sim_element(new_element.id());
            fault_areas[sit->second.sid()] += this_element.area();

            // If we want to taper, keep a record of the elements at each distance along strike.
            // Convention: I will be using the minimum das
            if (taper_method == "taper" || taper_method == "taper_renorm") {
                UIndex this_fault = eqsim_world.section(new_element.section_id()).fault_id();
                faults_with_elements_at_each_das[this_fault][this_element.min_das()].insert(new_element.id());
            }
        }

    }

    // Go through the created elements and assign maximum slip based on fault section area.
    for (eit=eqsim_world.begin_element(); eit!=eqsim_world.end_element(); ++eit) {
        // From Table 2A in Wells Coppersmith 1994
        //double moment_magnitude = 4.07+0.98*log10(conv.sqm2sqkm(fault_areas[eit->section_id()]));

        // Schultz: Updating these to a newer paper. From Leonard 2010
        double moment_magnitude = 4.0+log10(conv.sqm2sqkm(fault_areas[eit->section_id()]));

        // Need to document where this scaling law comes from
        double max_slip = pow(10, (3.0/2.0)*(moment_magnitude+6.0))/((eit->lame_mu())*fault_areas[eit->section_id()]);

        // Set the max slip for the current element
        eit->set_max_slip(max_slip);

        taper_t = 1;

        // ---------- Taper the slip rates ------------------
        if (taper_method == "taper" || taper_method == "taper_renorm") {

            // Loop over all other elements to compute the max depth for other elements above and below the current element.
            // (alternatively stated, those elements with the same distance along strike on the same fault)
            double max_depth_at_das = -DBL_MAX;
            quakelib::SimElement this_element = eqsim_world.create_sim_element(eit->id());
            UIndex this_fault = eqsim_world.section(eit->section_id()).fault_id();

            for (id_it=faults_with_elements_at_each_das[this_fault][this_element.min_das()].begin(); id_it!=faults_with_elements_at_each_das[this_fault][this_element.min_das()].end(); ++id_it) {
                quakelib::SimElement that_element = eqsim_world.create_sim_element(*id_it);
                max_depth_at_das = fmax(max_depth_at_das, fabs(that_element.max_depth()));
            }

            // The slip rate will be reduced by an amount corresponding to its depth_along_dip relative to the maximum
            //   at this position along strike.
            double adjusted_dip = (this_element.dip() <= M_PI/2 ) ? this_element.dip() : M_PI - this_element.dip();
            double this_depth = 0.5*fabs(this_element.max_depth() + this_element.min_depth());
            double this_depth_down_dip = this_depth/sin(adjusted_dip);
            double max_depth_down_dip_at_das = max_depth_at_das/sin(adjusted_dip);

            double z = this_depth_down_dip/max_depth_down_dip_at_das;
            taper_t *= sqrt(1-z);

            //Wilson: Removing horizontal dependance in tapering.
            /*
            if (taper_method == "taper_full" || taper_method == "taper_renorm") {
                double x = mid_t;
                double z = (float(ve)+0.5)/num_vert_elems;
                taper_t *= 4*(x-x*x)*sqrt(1-z);
            } else if (taper_method == "taper") {
                double inside_dist = (0.5 - fabs(0.5-mid_t));

                if (inside_dist <= elem_depth) {
                    double x = inside_dist/elem_depth;
                    double z = (float(ve)+0.5)/num_vert_elems;
                    taper_t *= sqrt(x)*sqrt(1-z);
                }
            }*/



        }

        section_taper_flow[eit->section_id()] += taper_t *eit->slip_rate()*eqsim_world.create_sim_element(eit->id()).area();
        section_taper_full[eit->section_id()] += eit->slip_rate()*eqsim_world.create_sim_element(eit->id()).area();

        // Adjust the slip rate
        eit->set_slip_rate(eit->slip_rate()*taper_t);

    }

    // Renormalize the slip rates to preserve total moment rate
    if (taper_method == "taper_renorm") {
        double cur_slip_rate;

        for (eit=eqsim_world.begin_element(); eit!=eqsim_world.end_element(); ++eit) {
            double renorm_factor = section_taper_full[eit->section_id()]/section_taper_flow[eit->section_id()];
            cur_slip_rate = eit->slip_rate();
            eit->set_slip_rate(renorm_factor*cur_slip_rate);
        }
    }


    insert(eqsim_world);

    return 0;
}

int quakelib::ModelWorld::write_files_eqsim(const std::string &geom_file_name, const std::string &cond_file_name, const std::string &fric_file_name) {
    EQSimGeometryWriter     geometry_data;
    EQSimFrictionWriter     friction_data;
    eiterator               eit;
    siterator               sit;
    UIndex                  sid;
    UIndex                  vind, eind;
    LatLonDepth             base = min_bound();
    Conversion              c(base);

    vind = eind = 1;
    friction_data.set_lame_lambda_mu(3.2e10, 3.0e10);

    for (sit=begin_section(); sit!=end_section(); ++sit) {
        EQSimGeometrySection &section = geometry_data.new_section();

        // Set section properties
        section.set_name(sit->name());
        section.set_fid(sit->fault_id());
        section.set_sid(sit->id());

        sid = sit->id();

        for (eit=begin_element(sid); eit!=end_element(sid); ++eit) {
            // Create SimElement to allow dip calculation
            SimElement      sim_elem = create_sim_element(eit->id());

            // Create EQSim rectangle and vertices
            EQSimGeometryRectangle  &elem = section.new_rectangle();
            EQSimGeometryVertex &v0 = section.new_vertex();
            v0.set_index(vind++);
            EQSimGeometryVertex &v1 = section.new_vertex();
            v1.set_index(vind++);
            EQSimGeometryVertex &v2 = section.new_vertex();
            v2.set_index(vind++);
            EQSimGeometryVertex &v3 = section.new_vertex();
            v3.set_index(vind++);

            // Set element properties
            friction_data.set_strengths(eind, 2.261600e+007, 0);
            elem.set_index(eind++);
            elem.set_vertex(0, v0.index());
            elem.set_vertex(1, v1.index());
            elem.set_vertex(2, v2.index());
            elem.set_vertex(3, v3.index());
            elem.set_rake(c.rad2deg(eit->rake()));
            elem.set_slip_rate(eit->slip_rate());
            elem.set_aseismic(eit->aseismic());
            // TODO: set strike
            elem.set_dip(c.rad2deg(sim_elem.dip()));
            elem.set_perfect_flag(1);

            // Set vertex properties
            v0.set_loc(vertex(eit->vertex(0)).lld());
            v0.set_das(vertex(eit->vertex(0)).das());

            switch (vertex(eit->vertex(0)).is_trace()) {
                case 0:
                    v0.set_trace_flag(NOT_ON_TRACE);
                    break;

                case 1:
                    v0.set_trace_flag(MIDDLE_TRACE);
                    break;

                case 2:
                    v0.set_trace_flag(BEGINNING_TRACE);
                    break;

                case 3:
                    v0.set_trace_flag(END_TRACE);
                    break;

                default:
                    v0.set_trace_flag(UNDEFINED_TRACE_STATUS);
                    break;
            }

            v1.set_loc(vertex(eit->vertex(1)).lld());
            v1.set_das(vertex(eit->vertex(1)).das());
            v1.set_trace_flag(NOT_ON_TRACE);

            v2.set_loc(c.convert2LatLon(sim_elem.implicit_vert()));
            v2.set_das(vertex(eit->vertex(2)).das());
            v2.set_trace_flag(NOT_ON_TRACE);

            v3.set_loc(vertex(eit->vertex(2)).lld());
            v3.set_das(vertex(eit->vertex(2)).das());

            switch (vertex(eit->vertex(2)).is_trace()) {
                case 0:
                    v3.set_trace_flag(NOT_ON_TRACE);
                    break;

                case 1:
                    v3.set_trace_flag(MIDDLE_TRACE);
                    break;

                case 2:
                    v3.set_trace_flag(BEGINNING_TRACE);
                    break;

                case 3:
                    v3.set_trace_flag(END_TRACE);
                    break;

                default:
                    v3.set_trace_flag(UNDEFINED_TRACE_STATUS);
                    break;
            }
        }
    }

    geometry_data.open(geom_file_name);
    geometry_data.write();
    geometry_data.close();

    friction_data.open(fric_file_name);
    friction_data.write();
    friction_data.close();

    return 0;
}

int quakelib::ModelWorld::write_file_kml(const std::string &file_name) {
    std::ofstream                                   out_file;
    std::map<UIndex, ModelFault>::const_iterator    fit;
    std::map<UIndex, ModelSection>::const_iterator  sit;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    LatLonDepth                                     min_bound, max_bound, center;
    Vec<3>                                          min_xyz, max_xyz;
    double                                          dx, dy, range;
    unsigned int                                    i;

    out_file.open(file_name.c_str());

    get_bounds(min_bound, max_bound);
    center = LatLonDepth(max_bound.lat() - (max_bound.lat()-min_bound.lat())/2,
                         max_bound.lon() - (max_bound.lon()-min_bound.lon())/2);
    Conversion c(center);
    min_xyz = c.convert2xyz(min_bound);
    max_xyz = c.convert2xyz(max_bound);
    dx = max_xyz[0]-min_xyz[0];
    dy = max_xyz[1]-min_xyz[1];
    range = fmax(dx, dy) * (1.0/tan(c.deg2rad(30)));

    // Kasey: Instead of using absolute value of depth and essentially
    // reflecting the faults across the surface z=0, find max depth
    // and raise the entire fault model by that amount so it looks right.
    double max_depth = fabs(min_bound.altitude());

    out_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out_file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
    out_file << "<Document>\n";
    out_file << "<LookAt>\n";
    out_file << "\t<latitude>" << center.lat() << "</latitude>\n";
    out_file << "\t<longitude>" << center.lon() << "</longitude>\n";
    out_file << "\t<altitude>0</altitude>\n";
    out_file << "\t<range>" << range << "</range>\n";
    out_file << "\t<tilt>0</tilt>\n";
    out_file << "\t<heading>0</heading>\n";
    out_file << "\t<altitudeMode>absolute</altitudeMode>\n";
    out_file << "</LookAt>\n";
    out_file << "<Style id=\"sectionLabel\">\n";
    out_file << "\t<IconStyle>\n";
    out_file << "\t\t<Icon>\n";
    out_file << "\t\t\t<href>http://maps.google.com/mapfiles/kml/paddle/wht-blank.png</href>\n";
    out_file << "\t\t</Icon>\n";
    out_file << "\t</IconStyle>\n";
    out_file << "</Style>\n";
    out_file << "<Folder id=\"fault_names\">\n";

    for (sit=_sections.begin(); sit!=_sections.end(); ++sit) {
        out_file << "\t<Placemark id=\"section_" << sit->second.id() << "_label\">\n";
        out_file << "\t\t<name>" << sit->second.id() << " " << sit->second.name() << "</name>\n";
        out_file << "\t\t<styleUrl>#sectionLabel</styleUrl>\n";
        out_file << "\t\t<Point>\n";
        out_file << "\t\t\t<extrude>1</extrude>\n";
        out_file << "\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n";
        // Find the deepest element for this section
        UIndex          best_vertex;
        double          min_altitude = DBL_MAX, cur_alt;
        ModelVertex     vert;

        for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
            if (eit->second.section_id() == sit->second.id()) {
                for (i=0; i<3; ++i) {
                    cur_alt = _vertices.find(eit->second.vertex(i))->second.lld().altitude();

                    if (cur_alt < min_altitude) {
                        min_altitude = cur_alt;
                        best_vertex = eit->second.vertex(i);
                    }
                }
            }
        }

        vert = _vertices.find(best_vertex)->second;
        out_file << "\t\t\t<coordinates>" << vert.lld().lon() << "," << vert.lld().lat() << "," << max_depth + vert.lld().altitude() << "</coordinates>\n";
        out_file << "\t\t</Point>\n";
        out_file << "\t</Placemark>\n";
    }

    out_file << "</Folder>\n";
    out_file << "<Folder id=\"faults\">\n";


    // Loop thru elements to compute min/max slip rates for color bounds
    double max_rate = 0;
    double min_rate = 0;

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        max_rate = fmax( max_rate, eit->second.slip_rate());
        min_rate = fmin( min_rate, eit->second.slip_rate());
    }

    /////// Bounds for color in blue to red range, white in the middle
    double y_min = 0;
    double y_max = 255;
    double x_min = min_rate;
    double x_max = max_rate;

    // Go through the faults
    for (fit = _faults.begin(); fit!= _faults.end(); ++fit) {
        out_file << "\t<Folder id=\"fault_" << fit->first << "\">\n";
        out_file << "\t\t<name>" << fit->first << " " << fit->second.name() <<"</name>\n";

        // Go through the sections
        for (sit=_sections.begin(); sit!=_sections.end(); ++sit) {
            // Check if section belongs to this fault
            if (sit->second.fault_id() == fit->first) {
                // And output the elements for each section
                out_file << "\t\t<Folder id=\"section_" << sit->first << "\">\n";
                out_file << "\t\t\t<name>" << sit->first << " " << sit->second.name() << "</name>\n";

                for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
                    if (sit->first == eit->second.section_id()) {
                        LatLonDepth         lld[4];
                        unsigned int        i, npoints;

                        for (i=0; i<3; ++i) {
                            std::map<UIndex, ModelVertex>::const_iterator   it;
                            it = _vertices.find(eit->second.vertex(i));
                            lld[i] = it->second.lld();
                        }

                        // If this is a quad element, calculate the 4th implicit point
                        if (eit->second.is_quad()) {
                            Vec<3>              xyz[3];

                            for (i=0; i<3; ++i) xyz[i] = c.convert2xyz(lld[i]);

                            lld[3] = lld[2];
                            lld[2] = c.convert2LatLon(xyz[2]+(xyz[1]-xyz[0]));
                        }

                        // Compute blue to red color (RGB)
                        // Keep red scale (min is white, max is red) so red=255
                        // Blue and green are equal and vary from (255 for min vals to 0 for max vals)
                        int blue, green;
                        int red = y_max;

                        if (eit->second.slip_rate() == 0) {
                            blue = 0;
                            green = 0;
                            red = 0;
                        } else {
                            int interp_color = (int) linear_interp(eit->second.slip_rate(), x_min, x_max, y_min, y_max);
                            blue = y_max - interp_color;
                            green = blue;
                        }

                        // Output the KML format polygon for this element
                        out_file << "\t\t\t<Placemark>\n";
                        out_file << "\t\t\t<description>\n";
                        out_file << "Section name (id  " << sit->second.id() << "): " << sit->second.name() << "\n";
                        out_file << "Element #: " << eit->second.id() << "\n";
                        out_file << "DAS [km]: " << element_min_das(eit->first)/1000.0 << " to " << element_max_das(eit->first)/1000.0 << "\n";
                        out_file << "Slip rate: " << c.m_per_sec2cm_per_yr(eit->second.slip_rate()) << " cm/year\n";
                        out_file << "Rake: " << c.rad2deg(eit->second.rake()) << " degrees\n";
                        out_file << "Aseismic: " << eit->second.aseismic() << "\n";
                        out_file << "\t\t\t</description>\n";
                        out_file << "\t\t\t\t<Style>\n";
                        out_file << "\t\t\t\t\t<LineStyle>\n";
                        out_file << "\t\t\t\t\t\t<color>"<< rgb2hex(red, green, blue) <<"</color>\n";
                        out_file << "\t\t\t\t\t\t<width>1</width>\n";
                        out_file << "\t\t\t\t\t</LineStyle>\n";
                        out_file << "\t\t\t\t\t<PolyStyle>\n";
                        out_file << "\t\t\t\t\t\t<color>"<< rgb2hex(red, green, blue) <<"</color>\n";
                        out_file << "\t\t\t\t\t</PolyStyle>\n";
                        out_file << "\t\t\t\t</Style>\n";
                        //out_file << "\t\t\t<styleUrl>#baseStyle</styleUrl>\n";
                        out_file << "\t\t\t\t<Polygon>\n";
                        out_file << "\t\t\t\t\t<extrude>0</extrude>\n";
                        out_file << "\t\t\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n";
                        out_file << "\t\t\t\t\t<outerBoundaryIs>\n";
                        out_file << "\t\t\t\t\t\t<LinearRing>\n";
                        out_file << "\t\t\t\t\t\t\t<coordinates>\n";
                        npoints = (eit->second.is_quad() ? 4 : 3);

                        for (i=0; i<npoints+1; ++i) out_file << "\t\t\t\t\t\t\t" << lld[i%npoints].lon() << "," << lld[i%npoints].lat() << "," << max_depth + lld[i%npoints].altitude() << "\n";

                        out_file << "\t\t\t\t\t\t\t</coordinates>\n";
                        out_file << "\t\t\t\t\t\t</LinearRing>\n";
                        out_file << "\t\t\t\t\t</outerBoundaryIs>\n";
                        out_file << "\t\t\t\t</Polygon>\n";
                        out_file << "\t\t\t</Placemark>\n";
                    }
                }

                out_file << "\t\t</Folder>\n";
            }

        }

        out_file << "\t</Folder>\n";
    }

    out_file << "</Folder>\n";
    out_file << "</Document>\n";
    out_file << "</kml>\n";

    out_file.close();

    return 0;
}

// Schultz: Using these functions for color interpolation and RGB to HEX color conversions
double quakelib::ModelWorld::linear_interp(const double &x, const double &x_min, const double &x_max, const double &y_min, const double &y_max) const {
    return ((y_max - y_min)/(x_max - x_min) * (x - x_min)) + y_min;
}

char *quakelib::ModelWorld::rgb2hex(const int r, const int g, const int b) const {
    // Returning ABGR to work with KML format
    char *buf;
    size_t sz;
    sz = snprintf(NULL, 0, "FF%02X%02X%02X", b,g,r);
    buf = (char *)malloc(sz + 1);
    snprintf(buf, sz+1, "FF%02X%02X%02X",  b,g,r);
    return buf;
}

int quakelib::ModelWorld::write_event_kml(const std::string &file_name, const quakelib::ModelEvent &event) {
    std::ofstream                                   out_file;
    std::map<UIndex, ModelSection>::const_iterator  fit;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    LatLonDepth                                     min_bound, max_bound, center;
    Vec<3>                                          min_xyz, max_xyz;
    double                                          dx, dy, range, mean_slip, ev_year;
    unsigned int                                    i, trigger, ev_num;
    ElementIDSet                                    involved_elements;
    ElementIDSet                                    involved_faults;
    ElementIDSet                                    all_elements;
    ElementIDSet::iterator                          it;

    out_file.open(file_name.c_str());

    get_bounds(min_bound, max_bound);
    center = LatLonDepth(max_bound.lat() - (max_bound.lat()-min_bound.lat())/2,
                         max_bound.lon() - (max_bound.lon()-min_bound.lon())/2);
    Conversion c(center);
    min_xyz = c.convert2xyz(min_bound);
    max_xyz = c.convert2xyz(max_bound);
    dx = max_xyz[0]-min_xyz[0];
    dy = max_xyz[1]-min_xyz[1];
    range = fmax(dx, dy) * (1.0/tan(c.deg2rad(30)));

    double max_depth = fabs(min_bound.altitude());

    //// Get relevant event info
    involved_elements = event.getInvolvedElements();
    trigger = event.getEventTrigger();
    mean_slip = event.calcMeanSlip();
    ev_year = event.getEventYear();
    ev_num = event.getEventNumber();
    LatLonDepth trigger_lld = _vertices.find(_elements.find(trigger)->second.vertex(0))->second.lld();

    out_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out_file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
    out_file << "<Document>\n";
    out_file << "<LookAt>\n";
    out_file << "\t<latitude>" << trigger_lld.lat() << "</latitude>\n";
    out_file << "\t<longitude>" << trigger_lld.lon() << "</longitude>\n";
    out_file << "\t<altitude>0</altitude>\n";
    out_file << "\t<range>" << range << "</range>\n";
    out_file << "\t<tilt>0</tilt>\n";
    out_file << "\t<heading>0</heading>\n";
    out_file << "\t<altitudeMode>absolute</altitudeMode>\n";
    out_file << "</LookAt>\n";
    out_file << "<Style id=\"sectionLabel\">\n";
    out_file << "\t<IconStyle>\n";
    out_file << "\t\t<Icon>\n";
    out_file << "\t\t\t<href>http://maps.google.com/mapfiles/kml/paddle/wht-blank.png</href>\n";
    out_file << "\t\t</Icon>\n";
    out_file << "\t</IconStyle>\n";
    out_file << "</Style>\n";

    // Write event info above triggering element
    out_file << "\t<Placemark id=\"Event_" << ev_num << "_label\">\n";
    out_file << "\t\t<name>" << "Event "<< ev_num << "</name>\n";
    out_file << "\t\t<description>\n";
    out_file << "Trigger: Element #"<< trigger << " \n";
    out_file << "Mean slip [m] "<< mean_slip << " \n";
    out_file << "Event year "<< ev_year << " \n";
    out_file << "\t\t</description>\n";
    out_file << "\t\t<styleUrl>#sectionLabel</styleUrl>\n";
    out_file << "\t\t<Point>\n";
    out_file << "\t\t\t<extrude>1</extrude>\n";
    out_file << "\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n";
    // Find the deepest element for this section
    UIndex          best_vertex;
    double          min_altitude = DBL_MAX, cur_alt;
    ModelVertex     vert;

    eit = _elements.find(trigger);

    for (i=0; i<3; ++i) {
        cur_alt = _vertices.find(eit->second.vertex(i))->second.lld().altitude();

        if (cur_alt < min_altitude) {
            min_altitude = cur_alt;
            best_vertex = eit->second.vertex(i);
        }
    }

    vert = _vertices.find(best_vertex)->second;
    out_file << "\t\t\t<coordinates>" << vert.lld().lon() << "," << vert.lld().lat() << "," << max_depth + vert.lld().altitude() << "</coordinates>\n";
    out_file << "\t\t</Point>\n";
    out_file << "\t</Placemark>\n";


    // Loop thru elements to compute min/max slip rates for color bounds
    double max_slip = -DBL_MAX;
    double min_slip = DBL_MAX;

    for (it=involved_elements.begin(); it!=involved_elements.end(); ++it) {
        max_slip = fmax( max_slip, event.getEventSlip(*it));
        min_slip = fmin( min_slip, event.getEventSlip(*it));
        involved_faults.insert(_sections.find(_elements.find(*it)->second.section_id())->second.fault_id());
    }

    // Add all the elements from the involved sections, regardless if they
    // actually slipped in the event. The ones with no slip will fill out the
    // faults and be nearly transparent.
    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        if (involved_faults.count(_sections.find(eit->second.section_id())->second.fault_id())) {
            all_elements.insert(eit->first);
        }
    }

    /////// Bounds for color in blue to red range, white in the middle
    double y_min = -255;
    double y_max = 255;
    double x_max = fmax(fabs(max_slip), fabs(min_slip));
    double x_min = -x_max;

    out_file << "<Folder id=\"elements\">\n";

    // Go through the involved elements
    for (it=all_elements.begin(); it!=all_elements.end(); ++it) {
        LatLonDepth         lld[4];
        unsigned int        i, npoints;

        for (i=0; i<3; ++i) {
            std::map<UIndex, ModelVertex>::const_iterator   vit;
            vit = _vertices.find( _elements.find(*it)->second.vertex(i) );
            lld[i] = vit->second.lld();
        }

        // If this is a quad element, calculate the 4th implicit point
        if (_elements.find(*it)->second.is_quad()) {

            Vec<3>              xyz[3];

            for (i=0; i<3; ++i) xyz[i] = c.convert2xyz(lld[i]);

            lld[3] = lld[2];
            lld[2] = c.convert2LatLon(xyz[2]+(xyz[1]-xyz[0]));
        }

        // If the element is involved in the event..
        // Compute blue to red color (RGB)
        if (involved_elements.count(*it)) {
            int blue, green, red;
            int interp_color = (int) linear_interp(event.getEventSlip(*it), x_min, x_max, y_min, y_max);

            if (interp_color > 0) {
                // More red
                red = y_max;
                blue = y_max - interp_color;
                green = blue;
            } else {
                blue = y_max;
                red = y_max - abs(interp_color);
                green = red;
            }

            // Output the KML format polygon for this element
            out_file << "\t\t<Placemark>\n";
            out_file << "\t\t<description>\n";
            out_file << "Element #: " << *it << "\n";
            out_file << "Event slip [m]: " << event.getEventSlip(*it) << "\n";
            out_file << "DAS [km]: " << element_min_das(*it)/1000.0 << " to " << element_max_das(*it)/1000.0 << "\n";
            out_file << "\t\t</description>\n";
            out_file << "\t\t\t<Style>\n";
            out_file << "\t\t\t\t<LineStyle>\n";
            out_file << "\t\t\t\t\t<color>"<< rgb2hex(red, green, blue) <<"</color>\n";
            out_file << "\t\t\t\t\t<width>1</width>\n";
            out_file << "\t\t\t\t</LineStyle>\n";
            out_file << "\t\t\t\t<PolyStyle>\n";
            out_file << "\t\t\t\t\t<color>"<< rgb2hex(red, green, blue) <<"</color>\n";
            out_file << "\t\t\t\t</PolyStyle>\n";
            out_file << "\t\t\t</Style>\n";
            out_file << "\t\t\t<Polygon>\n";
            out_file << "\t\t\t\t<extrude>0</extrude>\n";
            out_file << "\t\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n";
            out_file << "\t\t\t\t<outerBoundaryIs>\n";
            out_file << "\t\t\t\t\t<LinearRing>\n";
            out_file << "\t\t\t\t\t\t<coordinates>\n";
            //TODO: Fix compatibility with triangular models when we have them
            npoints = (_elements.find(*it)->second.is_quad() ? 4 : 3);
            //npoints = 4;

            for (i=0; i<npoints+1; ++i) out_file << "\t\t\t\t\t\t\t" << lld[i%npoints].lon() << "," << lld[i%npoints].lat() << "," << max_depth + lld[i%npoints].altitude() << "\n";

            out_file << "\t\t\t\t\t\t</coordinates>\n";
            out_file << "\t\t\t\t\t</LinearRing>\n";
            out_file << "\t\t\t\t</outerBoundaryIs>\n";
            out_file << "\t\t\t</Polygon>\n";
            out_file << "\t\t</Placemark>\n";
        } else {
            // If this element wasn't involved in the event and is included to fill out the fault
            // Output the KML format polygon for this element
            out_file << "\t\t<Placemark>\n";
            out_file << "\t\t<description>\n";
            out_file << "Element #: " << *it << "\n";
            out_file << "Distance along strike [km]: " << element_min_das(*it)/1000.0 << " to " << element_max_das(*it)/1000.0 << "\n";
            out_file << "\t\t</description>\n";
            out_file << "\t\t\t<Style>\n";
            out_file << "\t\t\t\t<LineStyle>\n";
            out_file << "\t\t\t\t\t<color>66FFFFFF</color>\n";
            out_file << "\t\t\t\t\t<width>1</width>\n";
            out_file << "\t\t\t\t</LineStyle>\n";
            out_file << "\t\t\t\t<PolyStyle>\n";
            out_file << "\t\t\t\t\t<color>66FFFFFF</color>\n";
            out_file << "\t\t\t\t</PolyStyle>\n";
            out_file << "\t\t\t</Style>\n";
            out_file << "\t\t\t<Polygon>\n";
            out_file << "\t\t\t\t<extrude>0</extrude>\n";
            out_file << "\t\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n";
            out_file << "\t\t\t\t<outerBoundaryIs>\n";
            out_file << "\t\t\t\t\t<LinearRing>\n";
            out_file << "\t\t\t\t\t\t<coordinates>\n";
            //TODO: Fix compatibility with triangular models when we have them
            npoints = (_elements.find(*it)->second.is_quad() ? 4 : 3);
            //npoints = 4;

            for (i=0; i<npoints; ++i) out_file << "\t\t\t\t\t\t\t" << lld[i].lon() << "," << lld[i].lat() << "," << max_depth + lld[i].altitude() << "\n";

            out_file << "\t\t\t\t\t\t</coordinates>\n";
            out_file << "\t\t\t\t\t</LinearRing>\n";
            out_file << "\t\t\t\t</outerBoundaryIs>\n";
            out_file << "\t\t\t</Polygon>\n";
            out_file << "\t\t</Placemark>\n";
        }
    }

    out_file << "</Folder>\n";
    out_file << "</Document>\n";
    out_file << "</kml>\n";

    out_file.close();

    return 0;
}


void quakelib::ModelSection::apply_remap(const ModelRemapping &remap) {
    _data._id = remap.get_section_id(_data._id);
}

void quakelib::ModelElement::apply_remap(const ModelRemapping &remap) {
    unsigned int            i;

    _data._id = remap.get_element_id(_data._id);
    _data._section_id = remap.get_section_id(_data._section_id);

    for (i=0; i<3; ++i) _data._vertices[i] = remap.get_vertex_id(_data._vertices[i]);
}


void quakelib::ModelVertex::apply_remap(const ModelRemapping &remap) {
    _data._id = remap.get_vertex_id(_data._id);
}

quakelib::UIndex quakelib::ModelRemapping::get_section_id(const UIndex &orig_id) const {
    IndexRemap::const_iterator it=section_remap.find(orig_id);
    return (it==section_remap.end() ? orig_id : it->second);
};

quakelib::UIndex quakelib::ModelRemapping::get_element_id(const UIndex &orig_id) const {
    IndexRemap::const_iterator it=element_remap.find(orig_id);
    return (it==element_remap.end() ? orig_id : it->second);
};

quakelib::UIndex quakelib::ModelRemapping::get_vertex_id(const UIndex &orig_id) const {
    IndexRemap::const_iterator it=vertex_remap.find(orig_id);
    return (it==vertex_remap.end() ? orig_id : it->second);
};

// Returns true if the remapping will overwrite (and lose) some sections/elements/vertices, false otherwise
bool quakelib::ModelWorld::overwrite(const ModelRemapping &remap) {
    std::map<UIndex, ModelSection>::const_iterator  fit;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    std::map<UIndex, ModelVertex>::const_iterator   vit;
    std::set<UIndex>    remap_inds;
    UIndex              remap_ind;

    remap_inds.clear();

    for (fit=_sections.begin(); fit!=_sections.end(); ++fit) {
        remap_ind = remap.get_section_id(fit->first);

        if (remap_ind == INVALID_INDEX) remap_ind = fit->first;

        if (remap_inds.count(remap_ind) > 0) return true;

        remap_inds.insert(remap_ind);
    }

    remap_inds.clear();

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        remap_ind = remap.get_element_id(eit->first);

        if (remap_ind == INVALID_INDEX) remap_ind = eit->first;

        if (remap_inds.count(remap_ind) > 0) return true;

        remap_inds.insert(remap_ind);
    }

    remap_inds.clear();

    for (vit=_vertices.begin(); vit!=_vertices.end(); ++vit) {
        remap_ind = remap.get_section_id(vit->first);

        if (remap_ind == INVALID_INDEX) remap_ind = vit->first;

        if (remap_inds.count(remap_ind) > 0) return true;

        remap_inds.insert(remap_ind);
    }

    return false;
}

// Apply the specified remapping to the sections, elements and vertices in this world
// Depending on the remapping it is entirely possible for things to be overwritten, it is the responsibility of the user to check this
void quakelib::ModelWorld::apply_remap(const ModelRemapping &remap) {
    std::map<UIndex, ModelSection>::iterator    fit;
    std::map<UIndex, ModelElement>::iterator    eit;
    std::map<UIndex, ModelVertex>::iterator     vit;
    std::vector<UIndex>                         erased_sections, erased_elements, erased_vertices;
    std::vector<UIndex>::iterator               it;
    std::vector<ModelSection>                   section_set;
    std::vector<ModelElement>                   element_set;
    std::vector<ModelVertex>                    vertex_set;
    unsigned int                                i;

    // Pull out any sections that will be remapped
    for (fit=_sections.begin(); fit!=_sections.end(); ++fit) {
        if (remap.get_section_id(fit->first) != INVALID_INDEX) {
            section_set.push_back(fit->second);
            section_set.back().apply_remap(remap);
            erased_sections.push_back(fit->first);
        } else {
            fit->second.apply_remap(remap);
        }
    }

    // Remove the old sections from the map
    for (it=erased_sections.begin(); it!=erased_sections.end(); ++it) _sections.erase(*it);

    erased_sections.clear();

    // Pull out any elements that will be remapped
    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        if (remap.get_element_id(eit->first) != INVALID_INDEX) {
            element_set.push_back(eit->second);
            element_set.back().apply_remap(remap);
            erased_elements.push_back(eit->first);
        } else {
            eit->second.apply_remap(remap);
        }
    }

    // Remove the old elements from the map
    for (it=erased_elements.begin(); it!=erased_elements.end(); ++it) _elements.erase(*it);

    erased_elements.clear();

    // Pull out any vertices that will be remapped
    for (vit=_vertices.begin(); vit!=_vertices.end(); ++vit) {
        if (remap.get_vertex_id(vit->first) != INVALID_INDEX) {
            vertex_set.push_back(vit->second);
            vertex_set.back().apply_remap(remap);
            erased_vertices.push_back(vit->first);
        } else {
            vit->second.apply_remap(remap);
        }
    }

    // Remove the old vertices from the map
    for (it=erased_vertices.begin(); it!=erased_vertices.end(); ++it) _vertices.erase(*it);

    erased_vertices.clear();

    // Go through each stored section and put it back in the world
    for (i=0; i<section_set.size(); ++i) _sections.insert(std::make_pair(section_set[i].id(),section_set[i]));

    section_set.clear();

    // Go through each element, change the indices and put it back in the world
    for (i=0; i<element_set.size(); ++i) _elements.insert(std::make_pair(element_set[i].id(),element_set[i]));

    element_set.clear();

    // Go through each vertex, change the indices and put it back in the world
    for (i=0; i<vertex_set.size(); ++i) _vertices.insert(std::make_pair(vertex_set[i].id(),vertex_set[i]));

    vertex_set.clear();
}

quakelib::ModelRemapping quakelib::ModelWorld::remap_indices_contiguous(const UIndex &start_section_index,
                                                                        const UIndex &start_element_index,
                                                                        const UIndex &start_vertex_index) const {
    ModelRemapping                                  remap;
    std::map<UIndex, ModelSection>::const_iterator  sit;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    std::map<UIndex, ModelVertex>::const_iterator   vit;
    UIndex                                          cur_ind;

    // Change section indices to be in contiguous ascending order
    for (cur_ind=start_section_index,sit=_sections.begin(); sit!=_sections.end(); ++cur_ind,++sit) remap.remap_section(sit->first, cur_ind);

    // Change element indices to be in contiguous ascending order
    for (cur_ind=start_element_index,eit=_elements.begin(); eit!=_elements.end(); ++cur_ind,++eit) remap.remap_element(eit->first, cur_ind);

    // Change vertex indices to be in contiguous ascending order
    for (cur_ind=start_vertex_index,vit=_vertices.begin(); vit!=_vertices.end(); ++cur_ind,++vit) remap.remap_vertex(vit->first, cur_ind);

    return remap;
}

quakelib::ModelRemapping quakelib::ModelWorld::remove_duplicate_vertices_remap(void) const {
    ModelRemapping                                  remap;
    std::map<UIndex, ModelVertex>::const_iterator   vit;
    std::map<Vec<3>, UIndex>                        vert_map;
    std::map<Vec<3>, UIndex>::const_iterator        mit;

    for (vit=_vertices.begin(); vit!=_vertices.end(); ++vit) {
        Vec<3>  pos = vit->second.xyz();
        mit = vert_map.find(pos);

        if (mit!= vert_map.end()) {
            remap.remap_vertex(vit->first, mit->second);
        } else {
            vert_map.insert(std::make_pair(pos, vit->first));
        }
    }

    return remap;
}

quakelib::LatLonDepth quakelib::ModelWorld::min_bound(const UIndex &fid) const {
    std::map<UIndex, ModelElement>::const_iterator  eit;
    bool            check_elem_verts;
    unsigned int    i;
    double          min_lat, min_lon, min_alt;

    min_lat = min_lon = min_alt = DBL_MAX;

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        check_elem_verts = (fid == INVALID_INDEX) || (eit->second.section_id() == fid);

        if (check_elem_verts) {
            // TODO: for quads determine 4th implicit point and check it
            for (i=0; i<3; ++i) {
                const ModelVertex &v = _vertices.find(eit->second.vertex(i))->second;
                min_lat = fmin(min_lat, v.lld().lat());
                min_lon = fmin(min_lon, v.lld().lon());
                min_alt = fmin(min_alt, v.lld().altitude());
            }
        }
    }

    return LatLonDepth(min_lat, min_lon, min_alt);
}

quakelib::LatLonDepth quakelib::ModelWorld::max_bound(const UIndex &fid) const {
    std::map<UIndex, ModelElement>::const_iterator  eit;
    bool            check_elem_verts;
    unsigned int    i;
    double          max_lat, max_lon, max_alt;

    max_lat = max_lon = max_alt = DBL_MAX;

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        check_elem_verts = (fid == INVALID_INDEX) || (eit->second.section_id() == fid);

        if (check_elem_verts) {
            // TODO: for quads determine 4th implicit point and check it
            for (i=0; i<3; ++i) {
                const ModelVertex &v = _vertices.find(eit->second.vertex(i))->second;
                max_lat = fmax(max_lat, v.lld().lat());
                max_lon = fmax(max_lon, v.lld().lon());
                max_alt = fmax(max_alt, v.lld().altitude());
            }
        }
    }

    return LatLonDepth(max_lat, max_lon, max_alt);
}

// TODO: rebase vertices
void quakelib::ModelWorld::insert(const quakelib::ModelWorld &other_world) {
    UIndex                      next_section_ind, next_element_ind, next_vertex_ind;
    ModelRemapping              remap;
    std::vector<ModelFault>   	fault_list;
    std::vector<ModelSection>   section_list;
    std::vector<ModelElement>   element_list;
    std::vector<ModelVertex>    vertex_list;
    unsigned int                i;
    std::map<UIndex, ModelFault>::const_iterator  fit;
    std::map<UIndex, ModelSection>::const_iterator  sit;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    std::map<UIndex, ModelVertex>::const_iterator   vit;
    LatLonDepth                 this_lld_min, this_lld_max;

    // Get the maximum indices in this model
    next_section_ind = next_section_index();
    next_element_ind = next_element_index();
    next_vertex_ind = next_vertex_index();

    // Create a remapping of the sections/elements/vertices in the
    // other model to allow it to be inserted in this model
    remap = other_world.remap_indices_contiguous(next_section_ind, next_element_ind, next_vertex_ind);

    // Make a copy of faults from other world, then insert into this world
	for (fit=other_world._faults.begin(); fit!=other_world._faults.end(); ++fit) {
		ModelFault  other_fault = fit->second;
		// TODO: Add fault remapping
		fault_list.push_back(other_fault);
	}

	for (i=0; i<fault_list.size(); ++i) _faults.insert(std::make_pair(fault_list[i].id(), fault_list[i]));

	fault_list.clear();

    // Make a copy of sections from other world, apply remapping, then insert into this world
    // Don't directly insert, since there's no guarantee that this world and the other aren't the same
    for (sit=other_world._sections.begin(); sit!=other_world._sections.end(); ++sit) {
        ModelSection  other_section = sit->second;
        other_section.apply_remap(remap);
        section_list.push_back(other_section);
    }

    for (i=0; i<section_list.size(); ++i) _sections.insert(std::make_pair(section_list[i].id(), section_list[i]));

    section_list.clear();

    // Insert elements from other world into this one, applying remapping
    for (eit=other_world._elements.begin(); eit!=other_world._elements.end(); ++eit) {
        ModelElement  other_element = eit->second;
        other_element.apply_remap(remap);
        element_list.push_back(other_element);
    }

    for (i=0; i<element_list.size(); ++i) _elements.insert(std::make_pair(element_list[i].id(), element_list[i]));

    element_list.clear();

    // Insert vertices from other world into this one, applying remapping
    for (vit=other_world._vertices.begin(); vit!=other_world._vertices.end(); ++vit) {
        ModelVertex     other_vertex = vit->second;
        other_vertex.apply_remap(remap);
        vertex_list.push_back(other_vertex);
    }

    for (i=0; i<vertex_list.size(); ++i) _vertices.insert(std::make_pair(vertex_list[i].id(), vertex_list[i]));

    // Get the new minimum latlon for the updated model and reset the vertices to match
    this->get_bounds(this_lld_min, this_lld_max);
    reset_base_coord(this_lld_min);
}

// TODO: correctly handle preexisting map entries
void quakelib::ModelWorld::insert(const quakelib::ModelFault &new_fault) {
    _faults.insert(std::make_pair(new_fault.id(), new_fault));
}

// TODO: correctly handle preexisting map entries
void quakelib::ModelWorld::insert(const quakelib::ModelSection &new_section) {
    _sections.insert(std::make_pair(new_section.id(), new_section));
}

// TODO: correctly handle preexisting map entries
void quakelib::ModelWorld::insert(const quakelib::ModelElement &new_element) {
    _elements.insert(std::make_pair(new_element.id(), new_element));
}

// TODO: correctly handle preexisting map entries
void quakelib::ModelWorld::insert(const quakelib::ModelVertex &new_vertex) {
    _vertices.insert(std::make_pair(new_vertex.id(), new_vertex));
}

double quakelib::ModelWorld::section_length(const quakelib::UIndex &sec_id) const {
    std::map<UIndex, ModelElement>::const_iterator  eit;
    double min_das = DBL_MAX;
    double max_das = -DBL_MAX;

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        if (eit->second.section_id() == sec_id) {
            min_das = fmin(min_das, create_sim_element(eit->first).min_das());
            max_das = fmax(max_das, create_sim_element(eit->first).max_das());
        }
    }

    return max_das-min_das;
}

double quakelib::ModelWorld::section_area(const quakelib::UIndex &sec_id) const {
    std::map<UIndex, ModelElement>::const_iterator  eit;
    double sec_area = 0.0;

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        if (eit->second.section_id() == sec_id) {
            sec_area += create_sim_element(eit->second.id()).area();
        }
    }

    return sec_area;
}

double quakelib::ModelWorld::section_max_depth(const quakelib::UIndex &sec_id) const {
    std::map<UIndex, ModelElement>::const_iterator  eit;
    double max_depth = DBL_MAX;

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        if (eit->second.section_id() == sec_id) {
            max_depth = fmin(max_depth, create_sim_element(eit->first).max_depth());
        }
    }

    return max_depth;
}

quakelib::Vec<3> quakelib::ModelWorld::element_mean_xyz(const quakelib::UIndex &id) const {
    quakelib::Vec<3> mean_xyz;
    mean_xyz += _vertices.find(_elements.find(id)->second.vertex(0))->second.xyz();
    mean_xyz += _vertices.find(_elements.find(id)->second.vertex(1))->second.xyz();
    mean_xyz += _vertices.find(_elements.find(id)->second.vertex(2))->second.xyz();
    mean_xyz += create_sim_element(id).implicit_vert();
    return mean_xyz/4.0;
}

double quakelib::ModelWorld::element_min_das(const quakelib::UIndex &id) const {
    return _vertices.find(_elements.find(id)->second.vertex(0))->second.das();
}

double quakelib::ModelWorld::element_max_das(const quakelib::UIndex &id) const {
    return _vertices.find(_elements.find(id)->second.vertex(2))->second.das();
}


size_t quakelib::ModelWorld::num_sections(void) const {
    return _sections.size();
}

size_t quakelib::ModelWorld::num_faults(void) const {
    std::set<UIndex>    fault_ids;
    std::map<UIndex, ModelSection>::const_iterator  sit;

    for (sit=_sections.begin(); sit!=_sections.end(); ++sit) {
        fault_ids.insert(sit->second.fault_id());
    }

    return fault_ids.size();
}

size_t quakelib::ModelWorld::num_elements(const quakelib::UIndex &fid) const {
    std::map<UIndex, ModelElement>::const_iterator  eit;
    size_t  num_elements = 0;

    if (fid == INVALID_INDEX) return _elements.size();

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        if (eit->second.section_id() == fid) num_elements++;
    }

    return num_elements;
}

quakelib::ElementIDSet quakelib::ModelWorld::getElementIDs(void) const {
    ElementIDSet element_id_set;
    std::map<UIndex, ModelElement>::const_iterator  eit;

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        element_id_set.insert(eit->second.id());
    }

    return element_id_set;
}

quakelib::ElementIDSet quakelib::ModelWorld::getVertexIDs(void) const {
    ElementIDSet vertex_id_set;
    std::map<UIndex, ModelVertex>::const_iterator  vit;

    for (vit=_vertices.begin(); vit!=_vertices.end(); ++vit) {
        vertex_id_set.insert(vit->second.id());
    }

    return vertex_id_set;
}

quakelib::ElementIDSet quakelib::ModelWorld::getSectionIDs(void) const {
    ElementIDSet sec_id_set;
    std::map<UIndex, ModelSection>::const_iterator  sit;

    for (sit=_sections.begin(); sit!=_sections.end(); ++sit) {
        sec_id_set.insert(sit->second.id());
    }

    return sec_id_set;
}

quakelib::ElementIDSet quakelib::ModelWorld::getFaultIDs(void) const {
    ElementIDSet fault_id_set;
    std::map<UIndex, ModelFault>::const_iterator  fit;

    for (fit=_faults.begin(); fit!=_faults.end(); ++fit) {
        fault_id_set.insert(fit->second.id());
    }

    return fault_id_set;
}

size_t quakelib::ModelWorld::num_vertices(const quakelib::UIndex &fid) const {
    std::set<UIndex>        section_vertices;
    std::map<UIndex, ModelElement>::const_iterator  eit;
    unsigned int            i;

    if (fid == INVALID_INDEX) return _vertices.size();

    for (eit=_elements.begin(); eit!=_elements.end(); ++eit) {
        if (eit->second.section_id() == fid) {
            for (i=0; i<3; ++i) section_vertices.insert(eit->second.vertex(i));
        }
    }

    return section_vertices.size();
}

void quakelib::ModelEvent::read_ascii(std::istream &in_stream) {
    std::stringstream   ss(next_line(in_stream));

    ss >> _data._event_number;
    ss >> _data._event_year;
    ss >> _data._event_trigger;
    ss >> _data._event_magnitude;
    ss >> _data._shear_stress_init;
    ss >> _data._shear_stress_final;
    ss >> _data._normal_stress_init;
    ss >> _data._normal_stress_final;
    ss >> _data._start_sweep_rec;
    ss >> _data._end_sweep_rec;
}

void quakelib::ModelEvent::write_ascii(std::ostream &out_stream) const {
    out_stream << _data._event_number << " ";
    out_stream << _data._event_year << " ";
    out_stream << _data._event_trigger << " ";
    out_stream << _data._event_magnitude << " ";
    out_stream << _data._shear_stress_init << " ";
    out_stream << _data._shear_stress_final << " ";
    out_stream << _data._normal_stress_init << " ";
    out_stream << _data._normal_stress_final << " ";
    out_stream << _data._start_sweep_rec << " ";
    out_stream << _data._end_sweep_rec;

    next_line(out_stream);
}

void quakelib::ModelSweeps::read_ascii(std::istream &in_stream, const unsigned int num_records) {
    for (unsigned int i=0; i<num_records; ++i) {
        std::stringstream   ss(next_line(in_stream));
        SweepData   new_sweep;
        ss >> new_sweep._event_number;
        ss >> new_sweep._sweep_number;
        ss >> new_sweep._element_id;
        ss >> new_sweep._slip;
        ss >> new_sweep._area;
        ss >> new_sweep._mu;
        ss >> new_sweep._shear_init;
        ss >> new_sweep._shear_final;
        ss >> new_sweep._normal_init;
        ss >> new_sweep._normal_final;
        // Record the sweep/element in the mapping
        std::pair<UIndex, UIndex> sweep_elem = std::make_pair(new_sweep._sweep_number, new_sweep._element_id);
        _rel.insert(std::make_pair(sweep_elem, _sweeps.size()));
        // Put the sweep on the list
        _sweeps.push_back(new_sweep);
    }
}

void quakelib::ModelSweeps::write_ascii(std::ostream &out_stream) const {
    std::vector<SweepData>::const_iterator it;

    for (it=_sweeps.begin(); it!=_sweeps.end(); ++it) {

        out_stream << it->_event_number << " ";
        out_stream << it->_sweep_number << " ";
        out_stream << it->_element_id << " ";
        out_stream << it->_slip << " ";
        out_stream << it->_area << " ";
        out_stream << it->_mu << " ";
        out_stream << it->_shear_init << " ";
        out_stream << it->_shear_final << " ";
        out_stream << it->_normal_init << " ";
        out_stream << it->_normal_final;

        next_line(out_stream);
    }
}

void quakelib::ModelSweeps::read_data(const SweepData &in_data) {
    // Record the sweep/element in the mapping
    std::pair<UIndex, UIndex> sweep_elem = std::make_pair(in_data._sweep_number, in_data._element_id);
    _rel.insert(std::make_pair(sweep_elem, _sweeps.size()));
    // Put the sweep on the list
    _sweeps.push_back(in_data);
}

void quakelib::ModelSweeps::write_data(SweepData &out_data) const {
    //memcpy(&out_data, &_data, sizeof(SweepData));
}

void quakelib::ModelEvent::read_data(const EventData &in_data) {
    memcpy(&_data, &in_data, sizeof(EventData));
}

void quakelib::ModelEvent::write_data(EventData &out_data) const {
    memcpy(&out_data, &_data, sizeof(EventData));
}

void quakelib::ModelSweeps::get_field_descs(std::vector<quakelib::FieldDesc> &descs) {
    FieldDesc       field_desc;

    // Sweep table definition
    field_desc.name = "event_number";
    field_desc.details = "Event number corresponding to this sweep.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _event_number);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);

    field_desc.name = "sweep_number";
    field_desc.details = "Sweep number within the event.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _sweep_number);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);

    field_desc.name = "block_id";
    field_desc.details = "Element ID.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _element_id);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);

    field_desc.name = "block_slip";
    field_desc.details = "Slip on element in this sweep (meters).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _slip);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "block_area";
    field_desc.details = "Area of element (square meters).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _area);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "block_mu";
    field_desc.details = "Element Lame mu parameter (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _mu);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "shear_init";
    field_desc.details = "Shear stress of element before sweep (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _shear_init);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "shear_final";
    field_desc.details = "Shear stress of element after sweep (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _shear_final);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "normal_init";
    field_desc.details = "Normal stress of element before sweep (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _normal_init);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "normal_final";
    field_desc.details = "Normal stress of element after sweep (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(SweepData, _normal_final);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);
}

void quakelib::ModelSweeps::write_ascii_header(std::ostream &out_stream) {
    std::vector<FieldDesc>                  descs;
    std::vector<FieldDesc>::const_iterator  dit;

    // Write section header
    ModelSweeps::get_field_descs(descs);

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_stream << "# " << dit->name << ": " << dit->details << "\n";
    }

    out_stream << "# ";

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_stream << dit->name << " ";
    }

    out_stream << "\n";
}

#ifdef HDF5_FOUND
void quakelib::ModelSweeps::setup_sweeps_hdf5(const hid_t &data_file) {
    std::vector<FieldDesc>  descs;
    size_t                  num_fields;
    unsigned int            i;
    SweepData               blank_sweep;
    char                    **field_names, **field_details;
    size_t                  *field_offsets;
    hid_t                   *field_types;
    size_t                  *field_sizes;
    herr_t                  res;

    // Set up the section table definition
    descs.clear();
    ModelSweeps::get_field_descs(descs);
    num_fields = descs.size();
    field_names = new char *[num_fields];
    field_details = new char *[num_fields];
    field_offsets = new size_t[num_fields];
    field_types = new hid_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_names[i] = new char[descs[i].name.length()+1];
        strncpy(field_names[i], descs[i].name.c_str(), descs[i].name.length());
        field_names[i][descs[i].name.length()] = '\0';
        field_details[i] = new char[descs[i].details.length()+1];
        strncpy(field_details[i], descs[i].details.c_str(), descs[i].details.length());
        field_details[i][descs[i].details.length()] = '\0';
        field_offsets[i] = descs[i].offset;
        field_types[i] = descs[i].type;
        field_sizes[i] = descs[i].size;
    }

    blank_sweep._event_number = blank_sweep._sweep_number = UNDEFINED_EVENT_ID;
    blank_sweep._element_id = UNDEFINED_ELEMENT_ID;
    blank_sweep._slip = blank_sweep._area = blank_sweep._mu = std::numeric_limits<float>::quiet_NaN();
    blank_sweep._shear_init = blank_sweep._shear_final = std::numeric_limits<float>::quiet_NaN();
    blank_sweep._normal_init = blank_sweep._normal_final = std::numeric_limits<float>::quiet_NaN();

    // Create the sweep table
    res = H5TBmake_table("Sweeps Table",
                         data_file,
                         ModelSweeps::hdf5_table_name().c_str(),
                         num_fields,
                         0,
                         sizeof(SweepData),
                         (const char **)field_names,
                         field_offsets,
                         field_types,
                         100,
                         &blank_sweep,
                         0,
                         NULL);

    if (res < 0) exit(-1);

    // Add the details of each field as an attribute
    for (i=0; i<num_fields; ++i) {
        std::stringstream   ss;
        ss << "FIELD_" << i << "_DETAILS";
        res = H5LTset_attribute_string(data_file, ModelSweeps::hdf5_table_name().c_str(), ss.str().c_str(), field_details[i]);

        if (res < 0) exit(-1);
    }

    // Free memory for HDF5 related data
    // yoder: ... and use delete [] for vector types...
    for (i=0; i<num_fields; ++i) delete [] field_names[i];

    delete [] field_names;

    for (i=0; i<num_fields; ++i) delete [] field_details[i];

    delete [] field_details;
    delete [] field_offsets;
    delete [] field_types;
    delete [] field_sizes;
}

void quakelib::ModelSweeps::append_sweeps_hdf5(const hid_t &data_file) const {
    std::vector<FieldDesc>                  descs;
    std::vector<SweepData>::const_iterator  it;
    size_t                                  num_fields, num_sweeps;
    unsigned int                i;
    SweepData                   *sweep_data;
    size_t                      *field_offsets;
    size_t                      *field_sizes;
    herr_t                      res;

    // Set up the section table definition
    descs.clear();
    ModelSweeps::get_field_descs(descs);
    num_fields = descs.size();
    num_sweeps = _sweeps.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    // Fill in the data for the sections
    sweep_data = new SweepData[num_sweeps];

    for (i=0,it=_sweeps.begin(); it!=_sweeps.end(); ++i,++it) {
        memcpy(&(sweep_data[i]), &(*it), sizeof(SweepData));
    }

    // Create the section table
    res = H5TBappend_records(data_file,
                             ModelSweeps::hdf5_table_name().c_str(),
                             num_sweeps,
                             sizeof(SweepData),
                             field_offsets,
                             field_sizes,
                             sweep_data);

    if (res < 0) exit(-1);

    // Free memory for HDF5 related data; use delete [] for array types...
    delete [] sweep_data;
    delete [] field_offsets;
    delete [] field_sizes;
}
#endif

void quakelib::ModelEvent::get_field_descs(std::vector<FieldDesc> &descs) {
    FieldDesc       field_desc;

    field_desc.name = "event_number";
    field_desc.details = "Event number.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _event_number);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);

    field_desc.name = "event_year";
    field_desc.details = "Event year.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _event_year);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "event_trigger";
    field_desc.details = "Event trigger element ID.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _event_trigger);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(UIndex);
#endif
    descs.push_back(field_desc);

    field_desc.name = "event_magnitude";
    field_desc.details = "Event magnitude.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _event_magnitude);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "event_shear_init";
    field_desc.details = "Total initial shear stress of elements involved in event (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _shear_stress_init);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "event_shear_final";
    field_desc.details = "Total final shear stress of elements involved in event (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _shear_stress_final);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "event_normal_init";
    field_desc.details = "Total initial normal stress of elements involved in event (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _normal_stress_init);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "event_normal_final";
    field_desc.details = "Total final normal stress of elements involved in event (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _normal_stress_final);
    field_desc.type = H5T_NATIVE_DOUBLE;
    field_desc.size = sizeof(double);
#endif
    descs.push_back(field_desc);

    field_desc.name = "start_sweep_rec";
    field_desc.details = "Starting record number of the sweeps comprising this event.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _start_sweep_rec);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);

    field_desc.name = "end_sweep_rec";
    field_desc.details = "Ending record number of the sweeps comprising this event.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(EventData, _end_sweep_rec);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);
}

void quakelib::ModelEvent::write_ascii_header(std::ostream &out_stream) {
    std::vector<FieldDesc>                  descs;
    std::vector<FieldDesc>::const_iterator  dit;

    // Write section header
    ModelEvent::get_field_descs(descs);

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_stream << "# " << dit->name << ": " << dit->details << "\n";
    }

    out_stream << "# ";

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_stream << dit->name << " ";
    }

    out_stream << "\n";
}

#ifdef HDF5_FOUND
void quakelib::ModelEvent::setup_event_hdf5(const hid_t &data_file) {
    std::vector<FieldDesc>  descs;
    size_t                  num_fields;
    unsigned int            i;
    EventData               blank_event;
    char                    **field_names, **field_details;
    size_t                  *field_offsets;
    hid_t                   *field_types;
    size_t                  *field_sizes;
    herr_t                  res;

    // Set up the section table definition
    descs.clear();
    ModelEvent::get_field_descs(descs);
    num_fields = descs.size();
    field_names = new char *[num_fields];
    field_details = new char *[num_fields];
    field_offsets = new size_t[num_fields];
    field_types = new hid_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_names[i] = new char[descs[i].name.length()+1];
        strncpy(field_names[i], descs[i].name.c_str(), descs[i].name.length());
        field_names[i][descs[i].name.length()] = '\0';
        field_details[i] = new char[descs[i].details.length()+1];
        strncpy(field_details[i], descs[i].details.c_str(), descs[i].details.length());
        field_details[i][descs[i].details.length()] = '\0';
        field_offsets[i] = descs[i].offset;
        field_types[i] = descs[i].type;
        field_sizes[i] = descs[i].size;
    }

    blank_event._event_number = UNDEFINED_EVENT_ID;
    blank_event._event_year = std::numeric_limits<float>::quiet_NaN();
    blank_event._event_magnitude = std::numeric_limits<float>::quiet_NaN();
    blank_event._event_trigger = UNDEFINED_ELEMENT_ID;
    blank_event._shear_stress_init = blank_event._shear_stress_final = std::numeric_limits<float>::quiet_NaN();
    blank_event._normal_stress_init = blank_event._normal_stress_final = std::numeric_limits<float>::quiet_NaN();
    blank_event._start_sweep_rec = blank_event._end_sweep_rec = UNDEFINED_EVENT_ID;

    // Create the event table
    res = H5TBmake_table("Event Table",
                         data_file,
                         ModelEvent::hdf5_table_name().c_str(),
                         num_fields,
                         0,
                         sizeof(EventData),
                         (const char **)field_names,
                         field_offsets,
                         field_types,
                         100,
                         &blank_event,
                         0,
                         NULL);

    if (res < 0) exit(-1);

    // Add the details of each field as an attribute
    for (i=0; i<num_fields; ++i) {
        std::stringstream   ss;
        ss << "FIELD_" << i << "_DETAILS";
        res = H5LTset_attribute_string(data_file,
                                       ModelEvent::hdf5_table_name().c_str(),
                                       ss.str().c_str(),
                                       field_details[i]);

        if (res < 0) exit(-1);
    }

    // Free memory for HDF5 related data
    // yoder: ... and delete [] for arrays...
    for (i=0; i<num_fields; ++i) delete [] field_names[i];

    delete [] field_names;

    for (i=0; i<num_fields; ++i) delete [] field_details[i];

    delete [] field_details;
    delete [] field_offsets;
    delete [] field_types;
    delete [] field_sizes;
}

void quakelib::ModelEvent::append_event_hdf5(const hid_t &data_file) const {
    std::vector<FieldDesc>  descs;
    size_t                  num_fields;
    unsigned int            i;
    size_t                  *field_offsets;
    size_t                  *field_sizes;
    herr_t                  res;

    // Set up the section table definition
    descs.clear();
    ModelEvent::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    // Append the event record
    res = H5TBappend_records(data_file,
                             ModelEvent::hdf5_table_name().c_str(),
                             1,
                             sizeof(EventData),
                             field_offsets,
                             field_sizes,
                             &(_data));

    if (res < 0) exit(-1);

    // Free memory for HDF5 related data
    delete [] field_offsets;
    delete [] field_sizes;
}
#endif

int quakelib::ModelEventSet::read_file_ascii(const std::string &event_file_name, const std::string &sweep_file_name) {
    std::ifstream   event_file, sweep_file;
    ModelSweeps     file_sweeps;

    // Try to open the event file
    event_file.open(event_file_name.c_str());

    if (!event_file.is_open()) return -1;

    // Try to open the sweeps file
    sweep_file.open(sweep_file_name.c_str());

    if (!sweep_file.is_open()) return -1;

    // Keep going until we hit the end of either file
    while (!event_file.eof() && !sweep_file.eof()) {
        ModelEvent  new_event;
        ModelSweeps new_sweeps;
        new_event.read_ascii(event_file);
        unsigned int num_rec_sweeps = new_event.getNumRecordedSweeps();
        new_sweeps.read_ascii(sweep_file, num_rec_sweeps);
        new_event.setSweeps(new_sweeps);

        if (!event_file.eof() && !sweep_file.eof()) _events.push_back(new_event);
    }

    // Close the files
    event_file.close();
    sweep_file.close();

    return 0;
}



int quakelib::ModelEventSet::read_file_hdf5(const std::string &file_name) {
#ifdef HDF5_FOUND
    hid_t       plist_id, data_file;
    herr_t      res;

    if (!H5Fis_hdf5(file_name.c_str())) return -1;

    plist_id = H5Pcreate(H5P_FILE_ACCESS);

    if (plist_id < 0) exit(-1);

    data_file = H5Fopen(file_name.c_str(), H5F_ACC_RDONLY, plist_id);

    if (data_file < 0) exit(-1);

    read_events_hdf5(data_file);
    read_sweeps_hdf5(data_file);

    // Release HDF5 handles
    res = H5Pclose(plist_id);

    if (res < 0) exit(-1);

    res = H5Fclose(data_file);

    if (res < 0) exit(-1);

#else
    // TODO: Error out
#endif
    return 0;

}

void quakelib::ModelEventSet::read_events_hdf5(const hid_t &data_file) {
#ifdef HDF5_FOUND
    std::vector<FieldDesc>                        descs;
    std::map<UIndex, ModelEvent>::const_iterator  fit;
    hsize_t                     num_fields, num_events;
    unsigned int                i;
    EventData                   *event_data;
    size_t                      *field_offsets;
    size_t                      *field_sizes;
    herr_t                      res;

    _events.clear();
    descs.clear();
    ModelEvent::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    res = H5TBget_table_info(data_file, ModelEvent::hdf5_table_name().c_str(), &num_fields, &num_events);

    if (res < 0) exit(-1);

    // TODO: check that num_fields matches the descs

    event_data = new EventData[num_events];
    res = H5TBread_records(data_file, ModelEvent::hdf5_table_name().c_str(), 0, num_events, sizeof(EventData), field_offsets, field_sizes, event_data);

    if (res < 0) exit(-1);

    // Read section data into the World
    for (i=0; i<num_events; ++i) {
        ModelEvent  new_event;
        new_event.read_data(event_data[i]);
        _events.push_back(new_event);
    }

    // Free memory for HDF5 related data
    delete [] event_data;
    delete [] field_offsets;
    delete [] field_sizes;

#else
    // TODO: Error out
#endif

}

void quakelib::ModelEventSet::read_sweeps_hdf5(const hid_t &data_file) {
#ifdef HDF5_FOUND
    std::vector<FieldDesc>                          descs;
    ModelEventSet::iterator                   fit;
    hsize_t                     num_fields, num_sweeps;
    unsigned int                i;
    unsigned int                start_sweep;
    unsigned int                end_sweep;
    SweepData                   *event_sweeps;
    size_t                      *field_offsets;
    size_t                      *field_sizes;
    herr_t                      res;

    descs.clear();
    ModelSweeps::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    res = H5TBget_table_info(data_file, ModelSweeps::hdf5_table_name().c_str(), &num_fields, &num_sweeps);

    if (res < 0) exit(-1);

    event_sweeps = new SweepData[num_sweeps];
    res = H5TBread_records(data_file, ModelSweeps::hdf5_table_name().c_str(), 0, num_sweeps, sizeof(SweepData), field_offsets, field_sizes, event_sweeps);

    if (res < 0) exit(-1);

    // Read sweeps data into the ModelEventSet
    for (fit=_events.begin(); fit!=_events.end(); ++fit) {
        fit->getStartEndSweep(start_sweep, end_sweep);
        ModelSweeps new_sweeps;

        for (i=start_sweep; i<end_sweep; i++) {
            new_sweeps.read_data(event_sweeps[i]);
        }

        fit->setSweeps(new_sweeps);

    }

    delete [] event_sweeps;
    // yoder: (added these deletes my self; are they supposed to not be deleted here and cleaned up somewhere else? looks like scope is wihtin function).
    delete [] field_offsets;
    delete [] field_sizes;
#else
    // TODO: Error out
#endif

}

int quakelib::ModelEventSet::append_from_hdf5(const std::string &file_name, const double &add_year, const unsigned int &add_evnum) {
#ifdef HDF5_FOUND
    hid_t       plist_id, data_file;
    herr_t      res;

    std::cout << "## Combining with events from " << file_name << std::endl;

    if (!H5Fis_hdf5(file_name.c_str())) return -1;

    plist_id = H5Pcreate(H5P_FILE_ACCESS);

    if (plist_id < 0) exit(-1);

    data_file = H5Fopen(file_name.c_str(), H5F_ACC_RDONLY, plist_id);

    if (data_file < 0) exit(-1);

    // Grab the largest event number currently in _events, we append events after this one
    unsigned int the_last_evnum = _events[_events.size()-1].getEventNumber();

    append_events_hdf5(data_file, add_year, add_evnum);
    append_sweeps_hdf5(data_file, the_last_evnum);

    // Release HDF5 handles
    res = H5Pclose(plist_id);

    if (res < 0) exit(-1);

    res = H5Fclose(data_file);

    if (res < 0) exit(-1);

#else
    // TODO: Error out
#endif
    return 0;
}

void quakelib::ModelEventSet::append_events_hdf5(const hid_t &data_file, const double &add_year, const unsigned int &add_evnum) {
#ifdef HDF5_FOUND
    std::vector<FieldDesc>                        descs;
    std::map<UIndex, ModelEvent>::const_iterator  fit;
    hsize_t                     num_fields, num_events;
    unsigned int                i;
    EventData                   *event_data;
    size_t                      *field_offsets;
    size_t                      *field_sizes;
    herr_t                      res;

    descs.clear();
    ModelEvent::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    res = H5TBget_table_info(data_file, ModelEvent::hdf5_table_name().c_str(), &num_fields, &num_events);

    if (res < 0) exit(-1);

    event_data = new EventData[num_events];
    res = H5TBread_records(data_file, ModelEvent::hdf5_table_name().c_str(), 0, num_events, sizeof(EventData), field_offsets, field_sizes, event_data);

    if (res < 0) exit(-1);

    unsigned int the_last_evnum = _events[_events.size()-1].getEventNumber();
    std::cout << "# Appending events after event number " << the_last_evnum <<  std::endl;

    for (i=0; i<num_events; ++i) {
        ModelEvent  new_event;
        new_event.read_data(event_data[i]);
        // Add the offsets for event numbers and years
        //std::cout << "Read event " << new_event.getEventNumber() << ", trigger = " << new_event.getEventTrigger() << std::endl;
        new_event.setEventNumber(new_event.getEventNumber()+add_evnum);
        new_event.setEventYear(new_event.getEventYear()+add_year);

        // Only add it if it's not in _events already
        if (new_event.getEventNumber() > the_last_evnum) {
            _events.push_back(new_event);
        }
    }

    // Free memory for HDF5 related data
    delete [] event_data;
    delete [] field_offsets;
    delete [] field_sizes;
#else
    // TODO: Error out
#endif
}

void quakelib::ModelEventSet::append_sweeps_hdf5(const hid_t &data_file, const unsigned int &last_evnum) {
#ifdef HDF5_FOUND
    std::vector<FieldDesc>                    descs;
    ModelEventSet::iterator                   fit;
    hsize_t                     num_fields, num_sweeps;
    unsigned int                i;
    unsigned int                start_sweep;
    unsigned int                end_sweep;
    SweepData                   *event_sweeps;
    size_t                      *field_offsets;
    size_t                      *field_sizes;
    herr_t                      res;

    descs.clear();
    ModelSweeps::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets = new size_t[num_fields];
    field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    res = H5TBget_table_info(data_file, ModelSweeps::hdf5_table_name().c_str(), &num_fields, &num_sweeps);

    if (res < 0) exit(-1);

    event_sweeps = new SweepData[num_sweeps];
    res = H5TBread_records(data_file, ModelSweeps::hdf5_table_name().c_str(), 0, num_sweeps, sizeof(SweepData), field_offsets, field_sizes, event_sweeps);

    if (res < 0) exit(-1);

    // Read sweeps data into the ModelEventSet
    for (fit=_events.begin(); fit!=_events.end(); ++fit) {
        // Do not modify sweeps for events already stored, only the newly added ones
        if (fit->getEventNumber() > last_evnum) {
            fit->getStartEndSweep(start_sweep, end_sweep);
            ModelSweeps new_sweeps;

            for (i=start_sweep; i<end_sweep; i++) {
                new_sweeps.read_data(event_sweeps[i]);
                new_sweeps.resetEventNumbers(fit->getEventNumber());
            }

            fit->setSweeps(new_sweeps);
        }
    }

    delete [] event_sweeps;
    // yoder: (added these deletes my self; are they supposed to not be deleted here and cleaned up somewhere else? looks like scope is wihtin function).
    delete [] field_offsets;
    delete [] field_sizes;

#else
    // TODO: Error out
#endif
}


// ********************************************************************************************

void quakelib::ModelStress::read_ascii(std::istream &in_stream, const unsigned int num_records) {
    for (unsigned int i=0; i<num_records; ++i) {
        std::stringstream   ss(next_line(in_stream));
        StressData     new_stress_rec;
        ss >> new_stress_rec._element_id;
        ss >> new_stress_rec._shear_stress;
        ss >> new_stress_rec._normal_stress;
        ss >> new_stress_rec._slip_deficit;
        // Put the stress record on the list
        _data.push_back(new_stress_rec);
    }
}

void quakelib::ModelStress::write_ascii(std::ostream &out_stream) const {
    std::vector<StressData>::const_iterator it;

    for (it=_data.begin(); it!=_data.end(); ++it) {
        out_stream << it->_element_id << " ";
        out_stream << it->_shear_stress << " ";
        out_stream << it->_normal_stress << " ";
        out_stream << it->_slip_deficit;

        next_line(out_stream);
    }
}

#ifdef HDF5_FOUND
void quakelib::ModelStress::setup_stress_hdf5(const hid_t &data_file) {
    std::vector<FieldDesc>  descs;
    size_t                  num_fields;
    unsigned int            i;
    StressData              blank_data;
    herr_t                  res;

    // Set up the section table definition
    descs.clear();
    ModelStress::get_field_descs(descs);
    num_fields = descs.size();
    char **field_names = new char *[num_fields];
    char **field_details = new char *[num_fields];
    size_t *field_offsets = new size_t[num_fields];
    hid_t *field_types = new hid_t[num_fields];
    size_t *field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_names[i] = new char[descs[i].name.length()+1];
        strncpy(field_names[i], descs[i].name.c_str(), descs[i].name.length());
        field_names[i][descs[i].name.length()] = '\0';
        field_details[i] = new char[descs[i].details.length()+1];
        strncpy(field_details[i], descs[i].details.c_str(), descs[i].details.length());
        field_details[i][descs[i].details.length()] = '\0';
        field_offsets[i] = descs[i].offset;
        field_types[i] = descs[i].type;
        field_sizes[i] = descs[i].size;
    }

    blank_data._element_id = UNDEFINED_ELEMENT_ID;
    blank_data._shear_stress = blank_data._normal_stress = std::numeric_limits<float>::quiet_NaN();

    // Create the sweep table
    res = H5TBmake_table("Stress Table",
                         data_file,
                         ModelStress::hdf5_table_name().c_str(),
                         num_fields,
                         0,
                         sizeof(StressData),
                         (const char **)field_names,
                         field_offsets,
                         field_types,
                         100,
                         &blank_data,
                         0,
                         NULL);

    if (res < 0) exit(-1);

    // Add the details of each field as an attribute
    for (i=0; i<num_fields; ++i) {
        std::stringstream   ss;
        ss << "FIELD_" << i << "_DETAILS";
        res = H5LTset_attribute_string(data_file, ModelStress::hdf5_table_name().c_str(), ss.str().c_str(), field_details[i]);

        if (res < 0) exit(-1);
    }

    // Free memory for HDF5 related data
    // yoder: ... and delete [] for arrays:
    for (i=0; i<num_fields; ++i) delete [] field_names[i];

    delete [] field_names;

    for (i=0; i<num_fields; ++i) delete [] field_details[i];

    delete [] field_details;
    delete [] field_offsets;
    delete [] field_types;
    delete [] field_sizes;
}

void quakelib::ModelStress::append_stress_hdf5(const hid_t &data_file) const {
    std::vector<FieldDesc>                  descs;
    std::vector<StressData>::const_iterator it;
    herr_t                      res;
    unsigned int                i;

    // Set up the section table definition
    descs.clear();
    ModelStress::get_field_descs(descs);
    size_t num_fields = descs.size();
    size_t num_stress_recs = _data.size();
    size_t *field_offsets = new size_t[num_fields];
    size_t *field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    // Fill in the data for the sections
    StressData *stress_data = new StressData[num_stress_recs];

    for (i=0,it=_data.begin(); it!=_data.end(); ++i,++it) {
        memcpy(&(stress_data[i]), &(*it), sizeof(StressData));
    }

    // Create the section table
    res = H5TBappend_records(data_file,
                             ModelStress::hdf5_table_name().c_str(),
                             num_stress_recs,
                             sizeof(StressData),
                             field_offsets,
                             field_sizes,
                             stress_data);

    if (res < 0) exit(-1);

    // Free memory for HDF5 related data
    delete [] stress_data;

    delete [] field_offsets;
    delete [] field_sizes;
}
#endif

void quakelib::ModelStress::write_ascii_header(std::ostream &out_stream) {
    std::vector<FieldDesc>                  descs;
    std::vector<FieldDesc>::const_iterator  dit;

    // Write section header
    ModelStress::get_field_descs(descs);

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_stream << "# " << dit->name << ": " << dit->details << "\n";
    }

    out_stream << "# ";

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_stream << dit->name << " ";
    }

    out_stream << "\n";
}

void quakelib::ModelStress::get_field_descs(std::vector<quakelib::FieldDesc> &descs) {
    FieldDesc       field_desc;

    // Stress state table definition
    field_desc.name = "element_id";
    field_desc.details = "ID number of the element corresponding to the stress values.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(StressData, _element_id);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);

    field_desc.name = "shear_stress";
    field_desc.details = "Shear stress on the element (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(StressData, _shear_stress);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "normal_stress";
    field_desc.details = "Normal stress on the element (Pascals).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(StressData, _normal_stress);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "slip_deficit";
    field_desc.details = "Slip deficit for the element (meters).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(StressData, _slip_deficit);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);
}

// ********************************************************************************************
#ifdef HDF5_FOUND
void quakelib::ModelStressState::read_data(const StressDataTime &in_data) {
    memcpy(&_times, &in_data, sizeof(StressDataTime));
}
#endif

void quakelib::ModelStressState::write_ascii_header(std::ostream &out_stream) {
    std::vector<FieldDesc>                  descs;
    std::vector<FieldDesc>::const_iterator  dit;

    // Write section header
    ModelStressState::get_field_descs(descs);

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_stream << "# " << dit->name << ": " << dit->details << "\n";
    }

    out_stream << "# ";

    for (dit=descs.begin(); dit!=descs.end(); ++dit) {
        out_stream << dit->name << " ";
    }

    out_stream << "\n";
}

#ifdef HDF5_FOUND
void quakelib::ModelStressState::setup_stress_state_hdf5(const hid_t &data_file) {
    std::vector<FieldDesc>  descs;
    unsigned int            i;
    StressDataTime          blank_data;
    herr_t                  res;

    // Set up the section table definition
    descs.clear();
    ModelStressState::get_field_descs(descs);
    size_t num_fields = descs.size();
    char **field_names = new char *[num_fields];
    char **field_details = new char *[num_fields];
    size_t *field_offsets = new size_t[num_fields];
    hid_t *field_types = new hid_t[num_fields];
    size_t *field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_names[i] = new char[descs[i].name.length()+1];
        strncpy(field_names[i], descs[i].name.c_str(), descs[i].name.length());
        field_names[i][descs[i].name.length()] = '\0';
        field_details[i] = new char[descs[i].details.length()+1];
        strncpy(field_details[i], descs[i].details.c_str(), descs[i].details.length());
        field_details[i][descs[i].details.length()] = '\0';
        field_offsets[i] = descs[i].offset;
        field_types[i] = descs[i].type;
        field_sizes[i] = descs[i].size;
    }

    blank_data._year = std::numeric_limits<float>::quiet_NaN();
    blank_data._event_num = UNDEFINED_EVENT_ID;
    //blank_data._sweep_num = UNDEFINED_EVENT_ID;
    blank_data._end_rec = UNDEFINED_ELEMENT_ID;
    blank_data._start_rec = UNDEFINED_ELEMENT_ID;

    // Create the sweep table
    res = H5TBmake_table("Stress State Table",
                         data_file,
                         ModelStressState::hdf5_table_name().c_str(),
                         num_fields,
                         0,
                         sizeof(StressDataTime),
                         (const char **)field_names,
                         field_offsets,
                         field_types,
                         100,
                         &blank_data,
                         0,
                         NULL);

    if (res < 0) exit(-1);

    // Add the details of each field as an attribute
    for (i=0; i<num_fields; ++i) {
        std::stringstream   ss;
        ss << "FIELD_" << i << "_DETAILS";
        res = H5LTset_attribute_string(data_file, ModelStressState::hdf5_table_name().c_str(), ss.str().c_str(), field_details[i]);

        if (res < 0) exit(-1);
    }

    // Free memory for HDF5 related data
    for (i=0; i<num_fields; ++i) delete [] field_names[i];

    delete [] field_names;

    for (i=0; i<num_fields; ++i) delete [] field_details[i];

    delete [] field_details;
    delete [] field_offsets;
    delete [] field_types;
    delete [] field_sizes;
}

void quakelib::ModelStressState::append_stress_state_hdf5(const hid_t &data_file) const {
    std::vector<FieldDesc>                  descs;
    std::vector<StressData>::const_iterator it;
    herr_t                      res;
    unsigned int                i;

    // Set up the section table definition
    descs.clear();
    ModelStressState::get_field_descs(descs);
    size_t num_fields = descs.size();
    size_t *field_offsets = new size_t[num_fields];
    size_t *field_sizes = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets[i] = descs[i].offset;
        field_sizes[i] = descs[i].size;
    }

    // Add to the stress state table
    res = H5TBappend_records(data_file,
                             ModelStressState::hdf5_table_name().c_str(),
                             1,
                             sizeof(StressDataTime),
                             field_offsets,
                             field_sizes,
                             &_times);

    if (res < 0) exit(-1);

    // Free memory for HDF5 related data
    delete [] field_offsets;
    delete [] field_sizes;
}
#endif


int quakelib::ModelStressSet::read_file_hdf5(const std::string &file_name) {
#ifdef HDF5_FOUND
    int                       plist_id, data_file;
    std::vector<FieldDesc>      descs;
    hsize_t                     num_fields, num_states, num_stresses;
    unsigned int                i, j;
    StressDataTime              *state_data;
    StressData                  *stress_data;
    size_t                      *field_offsets_state, *field_offsets_stress;
    size_t                      *field_sizes_state, *field_sizes_stress;
    herr_t                      res;

    if (!H5Fis_hdf5(file_name.c_str())) return -1;

    plist_id = H5Pcreate(H5P_FILE_ACCESS);

    if (plist_id < 0) exit(-1);

    data_file = H5Fopen(file_name.c_str(), H5F_ACC_RDONLY, plist_id);

    if (data_file < 0) exit(-1);

    // Read the state data from the file, grab the time data and number of stress records per state
    descs.clear();
    ModelStressState::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets_state = new size_t[num_fields];
    field_sizes_state = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets_state[i] = descs[i].offset;
        field_sizes_state[i] = descs[i].size;
    }

    res = H5TBget_table_info(data_file, ModelStressState::hdf5_table_name().c_str(), &num_fields, &num_states);

    if (res < 0) exit(-1);

    // Read all the stress time/num_recs data for all states
    state_data = new StressDataTime[num_states];
    res = H5TBread_records(data_file, ModelStressState::hdf5_table_name().c_str(), 0, num_states, sizeof(StressDataTime), field_offsets_state, field_sizes_state, state_data);

    // Prepare for stress table reading
    descs.clear();
    ModelStress::get_field_descs(descs);
    num_fields = descs.size();
    field_offsets_stress = new size_t[num_fields];
    field_sizes_stress = new size_t[num_fields];

    for (i=0; i<num_fields; ++i) {
        field_offsets_stress[i] = descs[i].offset;
        field_sizes_stress[i] = descs[i].size;
    }

    // Read all the stress data entries, we will parse them to the appropriate stress state later
    res = H5TBget_table_info(data_file, ModelStress::hdf5_table_name().c_str(), &num_fields, &num_stresses);

    if (res < 0) exit(-1);

    stress_data = new StressData[num_stresses];
    res = H5TBread_records(data_file, ModelStress::hdf5_table_name().c_str(), 0, num_stresses, sizeof(StressData), field_offsets_stress, field_sizes_stress, stress_data);

    // For each state, set the stress data
    for (i=0; i<num_states; ++i) {
        // Create the stress state, read the data
        ModelStressState new_state;
        new_state.read_data(state_data[i]);

        // Create this state's stress data, and read the corresponding stress records
        ModelStress new_stress;

        for (j=new_state.getStartRec(); j<new_state.getEndRec(); ++j) {
            new_stress.add_stress_entry(stress_data[j]);
        }

        // Set the stress data for the state
        new_state.setStresses(new_stress);
        // Add the state to the set of states
        _states.push_back(new_state);
    }

    // Release HDF5 handles
    res = H5Pclose(plist_id);

    if (res < 0) exit(-1);

    res = H5Fclose(data_file);

    if (res < 0) exit(-1);


    // Free memory for HDF5 related data
    // yoder: ... and use delete [] for arrays...
    delete [] stress_data;
    delete [] state_data;
    delete [] field_offsets_state;
    delete [] field_sizes_state;
    delete [] field_offsets_stress;
    delete [] field_sizes_stress;

#else
    // TODO: Error out
#endif
    return 0;
}


// Schultz: For the first version of the stress in/out, lets not write mid-event.
// If we write between events, then we don't need sweep info.
void quakelib::ModelStressState::read_ascii(std::istream &in_stream) {
    std::stringstream   ss(next_line(in_stream));
    ss >> _times._year;
    ss >> _times._event_num;
    //ss >> _times._sweep_num;
    ss >> _times._start_rec;
    ss >> _times._end_rec;
}

void quakelib::ModelStressState::write_ascii(std::ostream &out_stream) const {
    out_stream << _times._year << " ";
    out_stream << _times._event_num << " ";
    //out_stream << _times._sweep_num << " ";
    out_stream << _times._start_rec << " ";
    out_stream << _times._end_rec;

    next_line(out_stream);
}

void quakelib::ModelStressState::get_field_descs(std::vector<quakelib::FieldDesc> &descs) {
    FieldDesc       field_desc;

    // Stress state table definition
    field_desc.name = "year";
    field_desc.details = "Simulation time this stress state was recorded (years).";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(StressDataTime, _year);
    field_desc.type = H5T_NATIVE_FLOAT;
    field_desc.size = sizeof(float);
#endif
    descs.push_back(field_desc);

    field_desc.name = "event_num";
    field_desc.details = "Event number this stress state corresponds to.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(StressDataTime, _event_num);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);

    field_desc.name = "start_rec";
    field_desc.details = "Starting record of stress values for this time.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(StressDataTime, _start_rec);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);

    field_desc.name = "end_rec";
    field_desc.details = "Ending record of stress values for this time.";
#ifdef HDF5_FOUND
    field_desc.offset = HOFFSET(StressDataTime, _end_rec);
    field_desc.type = H5T_NATIVE_UINT;
    field_desc.size = sizeof(unsigned int);
#endif
    descs.push_back(field_desc);
}

int quakelib::ModelStressSet::read_file_ascii(const std::string &stress_index_file_name, const std::string &stress_file_name) {
    std::ifstream   stress_ind_file, stress_file;

    // Try to open the stress index file
    stress_ind_file.open(stress_index_file_name.c_str());

    if (!stress_ind_file.is_open()) return -1;

    // Try to open the stress file
    stress_file.open(stress_file_name.c_str());

    if (!stress_file.is_open()) return -1;

    // Keep going until we hit the end of either file
    while (!stress_ind_file.eof() && !stress_file.eof()) {
        ModelStressState    new_stress_state;
        ModelStress         new_stresses;
        new_stress_state.read_ascii(stress_ind_file);
        unsigned int num_recs = new_stress_state.getNumStressRecords();
        new_stresses.read_ascii(stress_file, num_recs);
        new_stress_state.setStresses(new_stresses);

        if (!stress_ind_file.eof() && !stress_file.eof()) _states.push_back(new_stress_state);
    }

    // Close the files
    stress_ind_file.close();
    stress_file.close();

    std::cout << "==== Read stress state from file ====" << std::endl;
    return 0;
}

namespace quakelib {
    std::ostream &operator<<(std::ostream &os, const ModelSweeps &ms) {
        os << "SWEEPS(" << ms._sweeps.size() << ")";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const ModelEvent &me) {
        os << me._data._event_number << " " << me._data._event_year;
        return os;
    }
}
