import React from "react";
import PropTypes from "prop-types";
import invariant from "tiny-invariant";
import warning from "tiny-warning";

import RouterContext from "./RouterContext.js";
import matchPath from "./matchPath.js";

/**
 * 用于呈现第一个匹配的 <Route> 的公共 API。
 */
class Switch extends React.Component {
    render() {
        return (
            <RouterContext.Consumer>
                {context => {
                    invariant(context, "You should not use <Switch> outside a <Router>");

                    const location = this.props.location || context.location;

                    let element, match;

                    // 我们在这里使用 React.Children.forEach 而不是 React.Children.toArray().find()
                    // 因为 toArray 会为所有子元素分别赋予不同的key属性，
                    // 我们不想因为多个组件有多个URL时，而创建多个不同的相同组件 触发unmount/remount。
                    React.Children.forEach(this.props.children, child => {
                        if (match == null && React.isValidElement(child)) {
                            element = child;

                            const path = child.props.path || child.props.from;
                            
                            match = path
                                ? matchPath(location.pathname, { ...child.props, path })
                                : context.match; //如果没有设置path，则默认使用context的match
                        }
                    });

                    //如果获取到匹配的Route，则对其element加上location和computedMatch这两个props
                    return match
                        ? React.cloneElement(element, { location, computedMatch: match })
                        : null;
                }}
            </RouterContext.Consumer>
        );
    }
}

if (__DEV__) {
    Switch.propTypes = {
        children: PropTypes.node,
        location: PropTypes.object
    };

    Switch.prototype.componentDidUpdate = function (prevProps) {
        warning(
            !(this.props.location && !prevProps.location),
            '<Switch> elements should not change from uncontrolled to controlled (or vice versa). You initially used no "location" prop and then provided one on a subsequent render.'
        );

        warning(
            !(!this.props.location && prevProps.location),
            '<Switch> elements should not change from controlled to uncontrolled (or vice versa). You provided a "location" prop initially but omitted it on a subsequent render.'
        );
    };
}

export default Switch;
